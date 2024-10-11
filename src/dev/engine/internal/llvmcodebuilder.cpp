// SPDX-License-Identifier: Apache-2.0

#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>

#include "llvmcodebuilder.h"
#include "llvmexecutablecode.h"

using namespace libscratchcpp;

static std::unordered_map<ValueType, Compiler::StaticType> TYPE_MAP = {
    { ValueType::Number, Compiler::StaticType::Number },           { ValueType::Bool, Compiler::StaticType::Bool },
    { ValueType::String, Compiler::StaticType::String },           { ValueType::Infinity, Compiler::StaticType::Number },
    { ValueType::NegativeInfinity, Compiler::StaticType::Number }, { ValueType::NaN, Compiler::StaticType::Number }
};

LLVMCodeBuilder::LLVMCodeBuilder(const std::string &id) :
    m_id(id),
    m_module(std::make_unique<llvm::Module>(id, m_ctx)),
    m_builder(m_ctx)
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    m_constValues.push_back({});
    m_regs.push_back({});
    initTypes();
}

std::shared_ptr<ExecutableCode> LLVMCodeBuilder::finalize()
{
    size_t functionIndex = 0;
    llvm::Function *currentFunc = beginFunction(functionIndex);
    std::vector<IfStatement> ifStatements;
    std::vector<Loop> loops;
    m_heap.clear();

    // Execute recorded steps
    for (const Step &step : m_steps) {
        switch (step.type) {
            case Step::Type::FunctionCall: {
                std::vector<llvm::Type *> types;
                std::vector<llvm::Value *> args;

                // Add target pointer arg
                assert(currentFunc->arg_size() == 1);
                types.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0));
                args.push_back(currentFunc->getArg(0));

                // Args
                for (auto &arg : step.args) {
                    types.push_back(getType(arg.first));
                    args.push_back(castValue(arg.second, arg.first));
                }

                llvm::Value *ret = m_builder.CreateCall(resolveFunction(step.functionName, llvm::FunctionType::get(getType(step.functionReturnType), types, false)), args);

                if (step.functionReturnReg) {
                    step.functionReturnReg->value = ret;

                    if (step.functionReturnType == Compiler::StaticType::String)
                        m_heap.push_back(step.functionReturnReg->value);
                }

                break;
            }

            case Step::Type::Yield:
                freeHeap();
                endFunction(currentFunc, functionIndex);
                currentFunc = beginFunction(++functionIndex);
                break;

            case Step::Type::BeginIf: {
                IfStatement statement;
                statement.beforeIf = m_builder.GetInsertBlock();
                statement.body = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                // Use last reg
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                freeHeap();
                statement.condition = castValue(reg.second, reg.first);

                // Switch to body branch
                m_builder.SetInsertPoint(statement.body);

                ifStatements.push_back(statement);
                break;
            }

            case Step::Type::BeginElse: {
                assert(!ifStatements.empty());
                IfStatement &statement = ifStatements.back();

                // Jump to the branch after the if statement
                assert(!statement.afterIf);
                statement.afterIf = llvm::BasicBlock::Create(m_ctx, "", currentFunc);
                freeHeap();
                m_builder.CreateBr(statement.afterIf);

                // Create else branch
                assert(!statement.elseBranch);
                statement.elseBranch = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                // Since there's an else branch, the conditional instruction should jump to it
                m_builder.SetInsertPoint(statement.beforeIf);
                m_builder.CreateCondBr(statement.condition, statement.body, statement.elseBranch);

                // Switch to the else branch
                m_builder.SetInsertPoint(statement.elseBranch);
                break;
            }

            case Step::Type::EndIf: {
                assert(!ifStatements.empty());
                IfStatement &statement = ifStatements.back();

                // Jump to the branch after the if statement
                if (!statement.afterIf)
                    statement.afterIf = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                freeHeap();
                m_builder.CreateBr(statement.afterIf);

                if (statement.elseBranch) {
                } else {
                    // If there wasn't an 'else' branch, create a conditional instruction which skips the if statement if false
                    m_builder.SetInsertPoint(statement.beforeIf);
                    m_builder.CreateCondBr(statement.condition, statement.body, statement.afterIf);
                }

                // Switch to the branch after the if statement
                m_builder.SetInsertPoint(statement.afterIf);

                ifStatements.pop_back();
                break;
            }

            case Step::Type::BeginRepeatLoop: {
                Loop loop;
                loop.isRepeatLoop = true;

                // index = 0
                llvm::Constant *zero = llvm::ConstantInt::get(m_builder.getInt64Ty(), 0, true);
                loop.index = m_builder.CreateAlloca(m_builder.getInt64Ty());
                m_builder.CreateStore(zero, loop.index);

                // Create branches
                llvm::BasicBlock *roundBranch = llvm::BasicBlock::Create(m_ctx, "", currentFunc);
                loop.conditionBranch = llvm::BasicBlock::Create(m_ctx, "", currentFunc);
                loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                // Use last reg for count
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Number);
                llvm::Value *count = castValue(reg.second, reg.first);

                // Clamp count if <= 0 (we can skip the loop if count is not positive)
                llvm::Value *comparison = m_builder.CreateFCmpULE(count, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)));
                freeHeap();
                m_builder.CreateCondBr(comparison, loop.afterLoop, roundBranch);

                // Round (Scratch-specific behavior)
                m_builder.SetInsertPoint(roundBranch);
                llvm::Function *roundFunc = llvm::Intrinsic::getDeclaration(m_module.get(), llvm::Intrinsic::round, { count->getType() });
                count = m_builder.CreateCall(roundFunc, { count });
                count = m_builder.CreateFPToSI(count, m_builder.getInt64Ty()); // cast to signed integer

                // Jump to condition branch
                m_builder.CreateBr(loop.conditionBranch);

                // Check index
                m_builder.SetInsertPoint(loop.conditionBranch);

                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                if (!loop.afterLoop)
                    loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
                comparison = m_builder.CreateICmpULT(currentIndex, count);
                m_builder.CreateCondBr(comparison, body, loop.afterLoop);

                // Switch to body branch
                m_builder.SetInsertPoint(body);

                loops.push_back(loop);
                break;
            }

            case Step::Type::BeginWhileLoop: {
                assert(!loops.empty());
                Loop &loop = loops.back();

                // Create branches
                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_ctx, "", currentFunc);
                loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                // Use last reg
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                llvm::Value *condition = castValue(reg.second, reg.first);
                freeHeap();
                m_builder.CreateCondBr(condition, body, loop.afterLoop);

                // Switch to body branch
                m_builder.SetInsertPoint(body);
                break;
            }

            case Step::Type::BeginRepeatUntilLoop: {
                assert(!loops.empty());
                Loop &loop = loops.back();

                // Create branches
                llvm::BasicBlock *body = llvm::BasicBlock::Create(m_ctx, "", currentFunc);
                loop.afterLoop = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                // Use last reg
                assert(step.args.size() == 1);
                const auto &reg = step.args[0];
                assert(reg.first == Compiler::StaticType::Bool);
                llvm::Value *condition = castValue(reg.second, reg.first);
                freeHeap();
                m_builder.CreateCondBr(condition, loop.afterLoop, body);

                // Switch to body branch
                m_builder.SetInsertPoint(body);
                break;
            }

            case Step::Type::BeginLoopCondition: {
                Loop loop;
                loop.isRepeatLoop = false;
                loop.conditionBranch = llvm::BasicBlock::Create(m_ctx, "", currentFunc);
                freeHeap();
                m_builder.CreateBr(loop.conditionBranch);
                m_builder.SetInsertPoint(loop.conditionBranch);
                loops.push_back(loop);
                break;
            }

            case Step::Type::EndLoop: {
                assert(!loops.empty());
                Loop &loop = loops.back();

                if (loop.isRepeatLoop) {
                    // Increment index
                    llvm::Value *currentIndex = m_builder.CreateLoad(m_builder.getInt64Ty(), loop.index);
                    llvm::Value *incremented = m_builder.CreateAdd(currentIndex, llvm::ConstantInt::get(m_builder.getInt64Ty(), 1, true));
                    m_builder.CreateStore(incremented, loop.index);
                }

                // Jump to the condition branch
                freeHeap();
                m_builder.CreateBr(loop.conditionBranch);

                // Switch to the branch after the loop
                m_builder.SetInsertPoint(loop.afterLoop);

                loops.pop_back();
                break;
            }
        }
    }

    freeHeap();

    endFunction(currentFunc, functionIndex);

#ifdef PRINT_LLVM_IR
    std::cout << std::endl << "=== LLVM IR (" << m_module->getName().str() << ") ===" << std::endl;
    m_module->print(llvm::outs(), nullptr);
    std::cout << "==============" << std::endl << std::endl;
#endif

    return std::make_shared<LLVMExecutableCode>(std::move(m_module));
}

void LLVMCodeBuilder::addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const std::vector<Compiler::StaticType> &argTypes)
{
    Step step(Step::Type::FunctionCall);
    step.functionName = functionName;

    assert(m_tmpRegs.size() >= argTypes.size());
    size_t j = 0;

    for (size_t i = m_tmpRegs.size() - argTypes.size(); i < m_tmpRegs.size(); i++)
        step.args.push_back({ argTypes[j++], m_tmpRegs[i] });

    m_tmpRegs.erase(m_tmpRegs.end() - argTypes.size(), m_tmpRegs.end());

    step.functionReturnType = returnType;

    if (returnType != Compiler::StaticType::Void) {
        auto reg = std::make_shared<Register>(returnType);
        reg->isRawValue = true;
        step.functionReturnReg = reg;
        m_regs[m_currentFunction].push_back(reg);
        m_tmpRegs.push_back(reg);
    }

    m_steps.push_back(step);
}

void LLVMCodeBuilder::addConstValue(const Value &value)
{
    auto reg = std::make_shared<Register>(TYPE_MAP[value.type()]);
    reg->isConstValue = true;
    reg->constValue = value;
    m_regs[m_currentFunction].push_back(reg);
    m_tmpRegs.push_back(reg);
}

void LLVMCodeBuilder::addVariableValue(Variable *variable)
{
}

void LLVMCodeBuilder::addListContents(List *list)
{
}

void LLVMCodeBuilder::beginIfStatement()
{
    Step step(Step::Type::BeginIf);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Bool, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginElseBranch()
{
    m_steps.push_back(Step(Step::Type::BeginElse));
}

void LLVMCodeBuilder::endIf()
{
    m_steps.push_back(Step(Step::Type::EndIf));
}

void LLVMCodeBuilder::beginRepeatLoop()
{
    Step step(Step::Type::BeginRepeatLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Number, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginWhileLoop()
{
    Step step(Step::Type::BeginWhileLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Bool, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginRepeatUntilLoop()
{
    Step step(Step::Type::BeginRepeatUntilLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back({ Compiler::StaticType::Bool, m_tmpRegs.back() });
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginLoopCondition()
{
    m_steps.push_back(Step(Step::Type::BeginLoopCondition));
}

void LLVMCodeBuilder::endLoop()
{
    m_steps.push_back(Step(Step::Type::EndLoop));
}

void LLVMCodeBuilder::yield()
{
    m_steps.push_back({ Step::Type::Yield });
    m_currentFunction++;

    assert(m_currentFunction == m_constValues.size());
    m_constValues.push_back({});

    assert(m_currentFunction == m_regs.size());
    m_regs.push_back({});
}

void LLVMCodeBuilder::initTypes()
{
    // Create the ValueData struct
    llvm::Type *doubleType = llvm::Type::getDoubleTy(m_ctx);                             // double (numberValue)
    llvm::Type *boolType = llvm::Type::getInt1Ty(m_ctx);                                 // bool (boolValue)
    llvm::Type *stringPtrType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0); // char* (stringValue)

    // Create the union type (largest type size should dominate)
    llvm::StructType *unionType = llvm::StructType::create(m_ctx, "union");
    unionType->setBody({ doubleType, boolType, stringPtrType });

    // Create the full struct type
    llvm::Type *valueType = llvm::Type::getInt32Ty(m_ctx); // Assuming ValueType is a 32-bit enum
    llvm::Type *sizeType = llvm::Type::getInt64Ty(m_ctx);  // size_t

    // Combine them into the full struct
    m_valueDataType = llvm::StructType::create(m_ctx, "ValueData");
    m_valueDataType->setBody({ unionType, valueType, sizeType });
}

llvm::Function *LLVMCodeBuilder::beginFunction(size_t index)
{
    // size_t f#(Target *)
    llvm::FunctionType *funcType = llvm::FunctionType::get(m_builder.getInt64Ty(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), false);
    llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "f" + std::to_string(index), m_module.get());

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_ctx, "entry", func);
    m_builder.SetInsertPoint(entry);

    return func;
}

void LLVMCodeBuilder::endFunction(llvm::Function *func, size_t index)
{
    if (!m_tmpRegs.empty()) {
        std::cout
            << "warning: " << m_tmpRegs.size() << " registers were leaked by script '" << m_module->getName().str() << "', function '" << func->getName().str()
            << "' (if you see this as a regular user, this is a bug and should be reported)" << std::endl;
    }

    // Return next function index
    m_builder.CreateRet(m_builder.getInt64(index + 1));

    if (llvm::verifyFunction(*func, &llvm::errs())) {
        llvm::errs() << "error: LLVM function verficiation failed!\n";
        llvm::errs() << "script hat ID: " << m_id << "\n";
        llvm::errs() << "function name: " << func->getName().data() << "\n";
    }
}

void LLVMCodeBuilder::freeHeap()
{
    // Free dynamically allocated memory
    for (llvm::Value *ptr : m_heap)
        m_builder.CreateFree(ptr);

    m_heap.clear();
}

llvm::Value *LLVMCodeBuilder::castValue(std::shared_ptr<Register> reg, Compiler::StaticType targetType)
{
    if (reg->isConstValue)
        return castConstValue(reg->constValue, targetType);

    if (reg->isRawValue)
        return castRawValue(reg, targetType);

    assert(reg->type != Compiler::StaticType::Void && targetType != Compiler::StaticType::Void);

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type) {
                case Compiler::StaticType::Number: {
                    // Read number directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                }

                case Compiler::StaticType::Bool: {
                    // Read boolean and cast to double
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    llvm::Value *boolValue = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                    return m_builder.CreateSIToFP(boolValue, m_builder.getDoubleTy());
                }

                case Compiler::StaticType::String: {
                    // Convert string to double
                    return m_builder.CreateCall(resolve_value_toDouble(), reg->value);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Bool:
            switch (reg->type) {
                case Compiler::StaticType::Number: {
                    // True if != 0
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    llvm::Value *numberValue = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                    return m_builder.CreateFCmpONE(numberValue, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)));
                }

                case Compiler::StaticType::Bool: {
                    // Read boolean directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                }

                case Compiler::StaticType::String:
                    // Convert string to bool
                    return m_builder.CreateCall(resolve_value_toBool(), reg->value);

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::String:
            switch (reg->type) {
                case Compiler::StaticType::Number:
                case Compiler::StaticType::Bool: {
                    // Cast to string
                    llvm::Value *ptr = m_builder.CreateCall(resolve_value_toCString(), reg->value);
                    m_heap.push_back(ptr); // deallocate later
                    return ptr;
                }

                case Compiler::StaticType::String: {
                    // Read string pointer directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), ptr);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        default:
            assert(false);
            return nullptr;
    }
}

llvm::Value *LLVMCodeBuilder::castRawValue(std::shared_ptr<Register> reg, Compiler::StaticType targetType)
{
    if (reg->type == targetType)
        return reg->value;

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type) {
                case Compiler::StaticType::Bool:
                    // Cast bool to double
                    return m_builder.CreateSIToFP(reg->value, m_builder.getDoubleTy());

                case Compiler::StaticType::String: {
                    // Convert string to double
                    return m_builder.CreateCall(resolve_value_stringToDouble(), reg->value);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Bool:
            switch (reg->type) {
                case Compiler::StaticType::Number:
                    // Cast double to bool (true if != 0)
                    return m_builder.CreateFCmpONE(reg->value, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)));

                case Compiler::StaticType::String:
                    // Convert string to bool
                    return m_builder.CreateCall(resolve_value_stringToBool(), reg->value);

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::String:
            switch (reg->type) {
                case Compiler::StaticType::Number: {
                    // Convert double to string
                    llvm::Value *ptr = m_builder.CreateCall(resolve_value_doubleToCString(), reg->value);
                    m_heap.push_back(ptr); // deallocate later
                    return ptr;
                }

                case Compiler::StaticType::Bool: {
                    // Convert bool to string
                    llvm::Value *ptr = m_builder.CreateCall(resolve_value_boolToCString(), reg->value);
                    // NOTE: Dot not deallocate later
                    return ptr;
                }

                default:
                    assert(false);
                    return nullptr;
            }

        default:
            assert(false);
            return nullptr;
    }
}

llvm::Value *LLVMCodeBuilder::castConstValue(const Value &value, Compiler::StaticType targetType)
{
    switch (targetType) {
        case Compiler::StaticType::Number:
            return llvm::ConstantFP::get(m_ctx, llvm::APFloat(value.toDouble()));

        case Compiler::StaticType::Bool:
            return m_builder.getInt1(value.toBool());

        case Compiler::StaticType::String:
            return m_builder.CreateGlobalStringPtr(value.toString());

        default:
            assert(false);
            return nullptr;
    }
}

llvm::Type *LLVMCodeBuilder::getType(Compiler::StaticType type)
{
    switch (type) {
        case Compiler::StaticType::Void:
            return m_builder.getVoidTy();

        case Compiler::StaticType::Number:
            return m_builder.getDoubleTy();

        case Compiler::StaticType::Bool:
            return m_builder.getInt1Ty();

        case Compiler::StaticType::String:
            return llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0);

        default:
            assert(false);
            return nullptr;
    }
}

llvm::FunctionCallee LLVMCodeBuilder::resolveFunction(const std::string name, llvm::FunctionType *type)
{
    return m_module->getOrInsertFunction(name, type);
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_init()
{
    return resolveFunction("value_init", llvm::FunctionType::get(m_builder.getVoidTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_free()
{
    return resolveFunction("value_free", llvm::FunctionType::get(m_builder.getVoidTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_long()
{
    return resolveFunction("value_assign_long", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_double()
{
    return resolveFunction("value_assign_double", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getDoubleTy() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_bool()
{
    return resolveFunction("value_assign_double", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt1Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_cstring()
{
    return resolveFunction("value_assign_cstring", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0) }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_assign_special()
{
    return resolveFunction("value_assign_special", llvm::FunctionType::get(m_builder.getVoidTy(), { m_valueDataType->getPointerTo(), m_builder.getInt32Ty() }, false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_toDouble()
{
    return resolveFunction("value_toDouble", llvm::FunctionType::get(m_builder.getDoubleTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_toBool()
{
    return resolveFunction("value_toBool", llvm::FunctionType::get(m_builder.getInt1Ty(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_toCString()
{
    return resolveFunction("value_toCString", llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_doubleToCString()
{
    return resolveFunction("value_doubleToCString", llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), m_builder.getDoubleTy(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_boolToCString()
{
    return resolveFunction("value_doubleToCString", llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), m_builder.getInt1Ty(), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_stringToDouble()
{
    return resolveFunction("value_stringToDouble", llvm::FunctionType::get(m_builder.getDoubleTy(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), false));
}

llvm::FunctionCallee LLVMCodeBuilder::resolve_value_stringToBool()
{
    return resolveFunction("value_stringToBool", llvm::FunctionType::get(m_builder.getInt1Ty(), llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0), false));
}
