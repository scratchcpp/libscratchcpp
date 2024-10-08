// SPDX-License-Identifier: Apache-2.0

#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <scratchcpp/value.h>

#include "llvmcodebuilder.h"
#include "llvmexecutablecode.h"

using namespace libscratchcpp;

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

LLVMCodeBuilder::~LLVMCodeBuilder()
{
    for (const auto &values : m_constValues) {
        for (const auto &v : values) {
            if (v)
                value_free(v.get());
        }
    }
}

std::shared_ptr<ExecutableCode> LLVMCodeBuilder::finalize()
{
    size_t functionIndex = 0;
    llvm::Function *currentFunc = beginFunction(functionIndex);
    std::vector<IfStatement> ifStatements;
    std::vector<Loop> loops;

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

                // Add return value arg if the function returns
                if (step.functionReturns) {
                    types.push_back(m_valueDataType->getPointerTo());
                    args.push_back(m_regs[functionIndex][step.functionReturnRegIndex]->value);
                }

                // Args
                for (auto &arg : step.args) {
                    types.push_back(m_valueDataType->getPointerTo());
                    args.push_back(arg->value);
                }

                m_builder.CreateCall(resolveFunction(step.functionName, llvm::FunctionType::get(m_builder.getVoidTy(), types, false)), args);
                break;
            }

            case Step::Type::Yield:
                endFunction(currentFunc, functionIndex);
                currentFunc = beginFunction(++functionIndex);
                break;

            case Step::Type::BeginIf: {
                IfStatement statement;
                statement.beforeIf = m_builder.GetInsertBlock();
                statement.body = llvm::BasicBlock::Create(m_ctx, "", currentFunc);

                // Convert last reg to bool
                assert(step.args.size() == 1);
                statement.condition = m_builder.CreateCall(resolve_value_toBool(), step.args[0]->value);

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

                // Convert last reg to double
                assert(step.args.size() == 1);
                llvm::Value *count = m_builder.CreateCall(resolve_value_toDouble(), step.args[0]->value);

                // Clamp count if <= 0 (we can skip the loop if count is not positive)
                llvm::Value *comparison = m_builder.CreateFCmpULE(count, llvm::ConstantFP::get(m_ctx, llvm::APFloat(0.0)));
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

                // Convert last reg to bool and add condition
                assert(step.args.size() == 1);
                llvm::Value *condition = m_builder.CreateCall(resolve_value_toBool(), step.args[0]->value);
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

                // Convert last reg to bool and add condition
                assert(step.args.size() == 1);
                llvm::Value *condition = m_builder.CreateCall(resolve_value_toBool(), step.args[0]->value);
                m_builder.CreateCondBr(condition, loop.afterLoop, body);

                // Switch to body branch
                m_builder.SetInsertPoint(body);
                break;
            }

            case Step::Type::BeginLoopCondition: {
                Loop loop;
                loop.isRepeatLoop = false;
                loop.conditionBranch = llvm::BasicBlock::Create(m_ctx, "", currentFunc);
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
                m_builder.CreateBr(loop.conditionBranch);

                // Switch to the branch after the loop
                m_builder.SetInsertPoint(loop.afterLoop);

                loops.pop_back();
                break;
            }
        }
    }

    endFunction(currentFunc, functionIndex);

#ifdef PRINT_LLVM_IR
    std::cout << std::endl << "=== LLVM IR (" << m_module->getName().str() << ") ===" << std::endl;
    m_module->print(llvm::outs(), nullptr);
    std::cout << "==============" << std::endl << std::endl;
#endif

    std::vector<std::unique_ptr<ValueData>> constValues;

    for (auto &values : m_constValues) {
        for (auto &v : values)
            constValues.push_back(std::move(v));
    }

    m_constValues.clear();
    return std::make_shared<LLVMExecutableCode>(std::move(m_module), constValues);
}

void LLVMCodeBuilder::addFunctionCall(const std::string &functionName, int argCount, bool returns)
{
    Step step(Step::Type::FunctionCall);
    step.functionName = functionName;

    assert(m_tmpRegs.size() >= argCount);

    for (size_t i = m_tmpRegs.size() - argCount; i < m_tmpRegs.size(); i++)
        step.args.push_back(m_tmpRegs[i]);

    m_tmpRegs.erase(m_tmpRegs.end() - argCount, m_tmpRegs.end());

    if (returns) {
        step.functionReturns = true;
        auto reg = std::make_shared<Register>();
        step.functionReturnRegIndex = m_regs[m_currentFunction].size();
        m_regs[m_currentFunction].push_back(reg);
        m_tmpRegs.push_back(reg);
    }

    m_steps.push_back(step);
}

void LLVMCodeBuilder::addConstValue(const Value &value)
{
    auto reg = std::make_shared<Register>();
    reg->isConstValue = true;
    reg->constValueIndex = m_constValues[m_currentFunction].size();
    m_regs[m_currentFunction].push_back(reg);
    m_tmpRegs.push_back(reg);

    std::unique_ptr<ValueData> v = std::make_unique<ValueData>();
    value_init(v.get());
    value_assign_copy(v.get(), &value.data());
    m_constValues[m_currentFunction].push_back(std::move(v));
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
    step.args.push_back(m_tmpRegs.back());
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
    step.args.push_back(m_tmpRegs.back());
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginWhileLoop()
{
    Step step(Step::Type::BeginWhileLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back(m_tmpRegs.back());
    m_tmpRegs.pop_back();
    m_steps.push_back(step);
}

void LLVMCodeBuilder::beginRepeatUntilLoop()
{
    Step step(Step::Type::BeginRepeatUntilLoop);
    assert(!m_tmpRegs.empty());
    step.args.push_back(m_tmpRegs.back());
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
    llvm::Type *intType = llvm::Type::getInt64Ty(m_ctx);                                 // long (intValue)
    llvm::Type *doubleType = llvm::Type::getDoubleTy(m_ctx);                             // double (doubleValue)
    llvm::Type *boolType = llvm::Type::getInt1Ty(m_ctx);                                 // bool (boolValue)
    llvm::Type *stringPtrType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_ctx), 0); // char* (stringValue)

    // Create the union type (largest type size should dominate)
    llvm::StructType *unionType = llvm::StructType::create(m_ctx, "union");
    unionType->setBody({ intType, doubleType, boolType, stringPtrType });

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

    // Add const value pointers
    const auto &constValues = m_constValues[index];
    std::vector<llvm::Value *> constPtrs;

    for (size_t i = 0; i < constValues.size(); i++) {
        llvm::Value *intAddress = m_builder.getInt64((uintptr_t)constValues[i].get());
        llvm::Value *ptr = m_builder.CreateIntToPtr(intAddress, m_valueDataType->getPointerTo());
        constPtrs.push_back(ptr);
    }

    // Add registers
    auto &regs = m_regs[index];
    size_t regIndex = 0;

    for (auto &reg : regs) {
        if (reg->isConstValue) {
            // Do not allocate space for existing constant values
            reg->value = constPtrs[reg->constValueIndex];
        } else {
            const std::string name = "r" + std::to_string(regIndex);

            llvm::Value *valueData = m_builder.CreateAlloca(m_valueDataType, nullptr, name);
            m_builder.CreateCall(resolve_value_init(), { valueData });

            reg->value = valueData;
            regIndex++;
        }
    }

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
