// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/target.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/blockprototype.h>
#include <scratchcpp/compiler.h>

#include "llvmbuildutils.h"
#include "llvmfunctions.h"
#include "llvmcompilercontext.h"
#include "llvmregister.h"
#include "llvminstruction.h"

using namespace libscratchcpp;

static std::unordered_map<ValueType, Compiler::StaticType> TYPE_MAP = {
    { ValueType::Number, Compiler::StaticType::Number },
    { ValueType::Bool, Compiler::StaticType::Bool },
    { ValueType::String, Compiler::StaticType::String },
    { ValueType::Pointer, Compiler::StaticType::Pointer }
};

static std::unordered_map<Compiler::StaticType, ValueType> REVERSE_TYPE_MAP = {
    { Compiler::StaticType::Number, ValueType::Number },
    { Compiler::StaticType::Bool, ValueType::Bool },
    { Compiler::StaticType::String, ValueType::String },
    { Compiler::StaticType::Pointer, ValueType::Pointer }
};

LLVMBuildUtils::LLVMBuildUtils(LLVMCompilerContext *ctx, llvm::IRBuilder<> &builder, Compiler::CodeType codeType) :
    m_ctx(ctx),
    m_llvmCtx(*ctx->llvmCtx()),
    m_builder(builder),
    m_functions(ctx, &builder),
    m_target(ctx->target()),
    m_codeType(codeType)
{
    initTypes();
    createVariableMap();
    createListMap();
}

void LLVMBuildUtils::init(llvm::Function *function, BlockPrototype *procedurePrototype, bool warp)
{
    m_function = function;
    m_procedurePrototype = procedurePrototype;
    m_warp = warp;

    m_executionContextPtr = m_function->getArg(0);
    m_targetPtr = m_function->getArg(1);
    m_targetVariables = m_function->getArg(2);
    m_targetLists = m_function->getArg(3);
    m_warpArg = m_procedurePrototype ? m_function->getArg(4) : nullptr;

    if (m_procedurePrototype && m_warp)
        m_function->addFnAttr(llvm::Attribute::InlineHint);

    m_stringHeap.clear();
    pushScopeLevel();

    // Init coroutine
    if (!m_warp)
        m_coroutine = std::make_unique<LLVMCoroutine>(m_ctx->module(), &m_builder, m_function);

    // Create variable pointers
    for (auto &[var, varPtr] : m_variablePtrs) {
        llvm::Value *ptr = getVariablePtr(m_targetVariables, var);
        varPtr.heapPtr = ptr;

        // Store variables locally to enable optimizations
        varPtr.stackPtr = m_builder.CreateAlloca(m_valueDataType);
    }

    // Create list pointers
    for (auto &[list, listPtr] : m_listPtrs) {
        listPtr.ptr = getListPtr(m_targetLists, list);

        listPtr.dataPtr = m_builder.CreateAlloca(m_valueDataType->getPointerTo()->getPointerTo());
        m_builder.CreateStore(m_builder.CreateCall(m_functions.resolve_list_data_ptr(), listPtr.ptr), listPtr.dataPtr);

        listPtr.sizePtr = m_builder.CreateCall(m_functions.resolve_list_size_ptr(), listPtr.ptr);
        listPtr.allocatedSizePtr = m_builder.CreateCall(m_functions.resolve_list_alloc_size_ptr(), listPtr.ptr);

        if (m_warp) {
            // Store list size locally to allow some optimizations
            listPtr.size = m_builder.CreateAlloca(m_builder.getInt64Ty(), nullptr, list->name() + ".size");

            llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
            m_builder.CreateStore(size, listPtr.size);

            // Store list type info locally to leave static type analysis to LLVM
            listPtr.hasNumber = m_builder.CreateAlloca(m_builder.getInt1Ty(), nullptr, list->name() + ".hasNumber");
            listPtr.hasBool = m_builder.CreateAlloca(m_builder.getInt1Ty(), nullptr, list->name() + ".hasBool");
            listPtr.hasString = m_builder.CreateAlloca(m_builder.getInt1Ty(), nullptr, list->name() + ".hasString");
        }
    }

    reloadVariables();
    reloadLists();

    // Create end branch
    m_endBranch = llvm::BasicBlock::Create(m_llvmCtx, "end", m_function);
}

void LLVMBuildUtils::end(LLVMInstruction *lastInstruction, LLVMRegister *lastConstant)
{
    assert(m_stringHeap.size() == 1);
    freeScopeHeap();

    m_builder.CreateBr(m_endBranch);

    m_builder.SetInsertPoint(m_endBranch);
    syncVariables();

    // End the script function
    llvm::PointerType *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_llvmCtx), 0);

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            if (m_warp)
                m_builder.CreateRet(llvm::ConstantPointerNull::get(pointerType));
            else
                m_coroutine->end();
            break;

        case Compiler::CodeType::Reporter: {
            // Use last instruction return value (or last constant) and create a ValueData instance
            assert(lastInstruction || lastConstant);
            LLVMRegister *ret = lastInstruction ? lastInstruction->functionReturnReg : lastConstant;
            llvm::Value *copy = createNewValue(ret);
            m_builder.CreateRet(m_builder.CreateLoad(m_valueDataType, copy));
            break;
        }

        case Compiler::CodeType::HatPredicate:
            // Use last instruction return value (or last constant)
            assert(lastInstruction || lastConstant);

            if (lastInstruction)
                m_builder.CreateRet(castValue(lastInstruction->functionReturnReg, Compiler::StaticType::Bool));
            else
                m_builder.CreateRet(castValue(lastConstant, Compiler::StaticType::Bool));
            break;
    }
}

LLVMCompilerContext *LLVMBuildUtils::compilerCtx() const
{
    return m_ctx;
}

llvm::LLVMContext &LLVMBuildUtils::llvmCtx()
{
    return m_llvmCtx;
}

llvm::Module *LLVMBuildUtils::module() const
{
    return m_ctx->module();
}

llvm::IRBuilder<> &LLVMBuildUtils::builder()
{
    return m_builder;
}

llvm::Function *LLVMBuildUtils::function() const
{
    return m_function;
}

LLVMFunctions &LLVMBuildUtils::functions()
{
    return m_functions;
}

std::string LLVMBuildUtils::scriptFunctionName(BlockPrototype *procedurePrototype)
{
    std::string name;

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            name = "script";
            break;

        case Compiler::CodeType::Reporter:
            name = "reporter";
            break;

        case Compiler::CodeType::HatPredicate:
            name = "predicate";
            break;
    }

    return procedurePrototype ? "proc." + procedurePrototype->procCode() : name;
}

llvm::FunctionType *LLVMBuildUtils::scriptFunctionType(BlockPrototype *procedurePrototype)
{
    // void *f(ExecutionContext *, Target *, ValueData **, List **, (warp arg), (procedure args...))
    // ValueData f(...) (reporters)
    // bool f(...) (hat predicates)
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_llvmCtx), 0);
    std::vector<llvm::Type *> argTypes = { pointerType, pointerType, pointerType, pointerType };

    if (procedurePrototype) {
        argTypes.push_back(m_builder.getInt1Ty()); // warp arg (only in procedures)
        const auto &types = procedurePrototype->argumentTypes();

        for (BlockPrototype::ArgType type : types) {
            if (type == BlockPrototype::ArgType::Bool)
                argTypes.push_back(m_builder.getInt1Ty());
            else
                argTypes.push_back(m_valueDataType->getPointerTo());
        }
    }

    llvm::Type *retType = nullptr;

    switch (m_codeType) {
        case Compiler::CodeType::Script:
            retType = pointerType;
            break;

        case Compiler::CodeType::Reporter:
            retType = m_valueDataType;
            break;

        case Compiler::CodeType::HatPredicate:
            retType = m_builder.getInt1Ty();
            break;
    }

    return llvm::FunctionType::get(retType, argTypes, false);
}

llvm::BasicBlock *LLVMBuildUtils::endBranch() const
{
    return m_endBranch;
}

BlockPrototype *LLVMBuildUtils::procedurePrototype() const
{
    return m_procedurePrototype;
}

bool LLVMBuildUtils::warp() const
{
    return m_warp;
}

llvm::Value *LLVMBuildUtils::executionContextPtr()
{
    return m_executionContextPtr;
}

llvm::Value *LLVMBuildUtils::targetPtr()
{
    return m_targetPtr;
}

llvm::Value *LLVMBuildUtils::targetVariables()
{
    return m_targetVariables;
}

llvm::Value *LLVMBuildUtils::targetLists()
{
    return m_targetLists;
}

llvm::Value *LLVMBuildUtils::warpArg()
{
    return m_warpArg;
}

LLVMCoroutine *LLVMBuildUtils::coroutine() const
{
    return m_coroutine.get();
}

void LLVMBuildUtils::createVariablePtr(Variable *variable)
{
    if (m_variablePtrs.find(variable) == m_variablePtrs.cend())
        m_variablePtrs[variable] = LLVMVariablePtr();
}

void LLVMBuildUtils::createListPtr(List *list)
{
    if (m_listPtrs.find(list) == m_listPtrs.cend())
        m_listPtrs[list] = LLVMListPtr();
}

LLVMVariablePtr &LLVMBuildUtils::variablePtr(Variable *variable)
{
    assert(m_variablePtrs.find(variable) != m_variablePtrs.cend());
    return m_variablePtrs[variable];
}

LLVMListPtr &LLVMBuildUtils::listPtr(List *list)
{
    assert(m_listPtrs.find(list) != m_listPtrs.cend());
    return m_listPtrs[list];
}

void LLVMBuildUtils::syncVariables()
{
    // Copy stack variables to the actual variables
    for (auto &[var, varPtr] : m_variablePtrs)
        createValueCopy(varPtr.stackPtr, getVariablePtr(m_targetVariables, var));
}

void LLVMBuildUtils::reloadVariables()
{
    // Load variables to stack
    for (auto &[var, varPtr] : m_variablePtrs) {
        llvm::Value *ptr = getVariablePtr(m_targetVariables, var);
        createValueCopy(ptr, varPtr.stackPtr);
    }
}

void LLVMBuildUtils::reloadLists()
{
    // Load list size and type info
    if (m_warp) {
        for (auto &[list, listPtr] : m_listPtrs) {
            llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
            m_builder.CreateStore(size, listPtr.size);

            m_builder.CreateStore(m_builder.getInt1(true), listPtr.hasNumber);
            m_builder.CreateStore(m_builder.getInt1(true), listPtr.hasBool);
            m_builder.CreateStore(m_builder.getInt1(true), listPtr.hasString);
        }
    }
}

void LLVMBuildUtils::pushScopeLevel()
{
    m_stringHeap.push_back({});
}

void LLVMBuildUtils::popScopeLevel()
{
    freeScopeHeap();
    m_stringHeap.pop_back();
}

void LLVMBuildUtils::freeStringLater(llvm::Value *value)
{
    assert(!m_stringHeap.empty());

    if (m_stringHeap.empty())
        return;

    m_stringHeap.back().push_back(value);
}

void LLVMBuildUtils::freeScopeHeap()
{
    if (m_stringHeap.empty())
        return;

    // Free strings in current scope
    auto &heap = m_stringHeap.back();

    for (llvm::Value *ptr : heap)
        m_builder.CreateCall(m_functions.resolve_string_pool_free(), { ptr });

    heap.clear();
}

std::vector<LLVMIfStatement> &LLVMBuildUtils::ifStatements()
{
    return m_ifStatements;
}

std::vector<LLVMLoop> &LLVMBuildUtils::loops()
{
    return m_loops;
}

Compiler::StaticType LLVMBuildUtils::optimizeRegisterType(const LLVMRegister *reg)
{
    Compiler::StaticType ret = reg->type();

    // Optimize string constants that represent numbers
    if (reg->isConst() && reg->type() == Compiler::StaticType::String && reg->constValue().isValidNumber())
        ret = Compiler::StaticType::Number;

    return ret;
}

Compiler::StaticType LLVMBuildUtils::mapType(ValueType type)
{
    assert(TYPE_MAP.find(type) != TYPE_MAP.cend());
    return TYPE_MAP[type];
}

ValueType LLVMBuildUtils::mapType(Compiler::StaticType type)
{
    assert(REVERSE_TYPE_MAP.find(type) != REVERSE_TYPE_MAP.cend());
    return REVERSE_TYPE_MAP[type];
}

bool LLVMBuildUtils::isSingleType(Compiler::StaticType type)
{
    // Check if the type is a power of 2 (only one bit set)
    return (type & (static_cast<Compiler::StaticType>(static_cast<int>(type) - 1))) == static_cast<Compiler::StaticType>(0);
}

llvm::Value *LLVMBuildUtils::addAlloca(llvm::Type *type)
{
    // Add an alloca to the entry block because allocas must be there (to avoid stack overflow)
    llvm::BasicBlock *block = m_builder.GetInsertBlock();
    m_builder.SetInsertPointPastAllocas(m_function);
    llvm::Value *ret = m_builder.CreateAlloca(type);
    m_builder.SetInsertPoint(block);
    return ret;
}

llvm::Value *LLVMBuildUtils::castValue(LLVMRegister *reg, Compiler::StaticType targetType)
{
    if (reg->isConst()) {
        if (!isSingleType(targetType))
            return createValue(reg);
        else
            return castConstValue(reg->constValue(), targetType);
    }

    if (reg->isRawValue)
        return castRawValue(reg, targetType);

    assert(reg->type() != Compiler::StaticType::Void && targetType != Compiler::StaticType::Void);

    llvm::Value *typePtr = nullptr;
    llvm::Value *loadedType = nullptr;
    llvm::BasicBlock *mergeBlock = nullptr;
    llvm::BasicBlock *defaultBlock = nullptr;
    llvm::SwitchInst *sw = nullptr;
    std::vector<std::pair<llvm::BasicBlock *, llvm::Value *>> results;

    if (isSingleType(targetType)) {
        // Handle multiple source type cases with runtime switch
        typePtr = getValueTypePtr(reg);
        loadedType = m_builder.CreateLoad(m_builder.getInt32Ty(), typePtr);

        mergeBlock = llvm::BasicBlock::Create(m_llvmCtx, "merge", m_function);
        defaultBlock = llvm::BasicBlock::Create(m_llvmCtx, "default", m_function);

        sw = m_builder.CreateSwitch(loadedType, defaultBlock, 4);
    }

    Compiler::StaticType type = reg->type();

    switch (targetType) {
        case Compiler::StaticType::Number: {
            // Number case
            if ((type & Compiler::StaticType::Number) == Compiler::StaticType::Number) {
                llvm::BasicBlock *numberBlock = llvm::BasicBlock::Create(m_llvmCtx, "number", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), numberBlock);

                m_builder.SetInsertPoint(numberBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *numberResult = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                m_builder.CreateBr(mergeBlock);
                results.push_back({ numberBlock, numberResult });
            }

            // Bool case
            if ((type & Compiler::StaticType::Bool) == Compiler::StaticType::Bool) {
                llvm::BasicBlock *boolBlock = llvm::BasicBlock::Create(m_llvmCtx, "bool", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), boolBlock);

                m_builder.SetInsertPoint(boolBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *boolValue = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                llvm::Value *boolResult = m_builder.CreateUIToFP(boolValue, m_builder.getDoubleTy());
                m_builder.CreateBr(mergeBlock);
                results.push_back({ boolBlock, boolResult });
            }

            // String case
            if ((type & Compiler::StaticType::String) == Compiler::StaticType::String) {
                llvm::BasicBlock *stringBlock = llvm::BasicBlock::Create(m_llvmCtx, "string", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), stringBlock);

                m_builder.SetInsertPoint(stringBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *stringPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
                llvm::Value *stringResult = m_builder.CreateCall(m_functions.resolve_value_stringToDouble(), stringPtr);
                m_builder.CreateBr(mergeBlock);
                results.push_back({ stringBlock, stringResult });
            }

            break;
        }

        case Compiler::StaticType::Bool: {
            // Number case
            if ((type & Compiler::StaticType::Number) == Compiler::StaticType::Number) {
                llvm::BasicBlock *numberBlock = llvm::BasicBlock::Create(m_llvmCtx, "number", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), numberBlock);

                m_builder.SetInsertPoint(numberBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *numberValue = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                llvm::Value *numberResult = m_builder.CreateFCmpONE(numberValue, llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0)));
                m_builder.CreateBr(mergeBlock);
                results.push_back({ numberBlock, numberResult });
            }

            // Bool case
            if ((type & Compiler::StaticType::Bool) == Compiler::StaticType::Bool) {
                llvm::BasicBlock *boolBlock = llvm::BasicBlock::Create(m_llvmCtx, "bool", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), boolBlock);

                m_builder.SetInsertPoint(boolBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *boolResult = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                m_builder.CreateBr(mergeBlock);
                results.push_back({ boolBlock, boolResult });
            }

            // String case
            if ((type & Compiler::StaticType::String) == Compiler::StaticType::String) {
                llvm::BasicBlock *stringBlock = llvm::BasicBlock::Create(m_llvmCtx, "string", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), stringBlock);

                m_builder.SetInsertPoint(stringBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *stringPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
                llvm::Value *stringResult = m_builder.CreateCall(m_functions.resolve_value_stringToBool(), stringPtr);
                m_builder.CreateBr(mergeBlock);
                results.push_back({ stringBlock, stringResult });
            }

            break;
        }

        case Compiler::StaticType::String: {
            // Number case
            if ((type & Compiler::StaticType::Number) == Compiler::StaticType::Number) {
                llvm::BasicBlock *numberBlock = llvm::BasicBlock::Create(m_llvmCtx, "number", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), numberBlock);

                m_builder.SetInsertPoint(numberBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *value = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                llvm::Value *numberResult = m_builder.CreateCall(m_functions.resolve_value_doubleToStringPtr(), value);
                m_builder.CreateBr(mergeBlock);
                results.push_back({ numberBlock, numberResult });
            }

            // Bool case
            if ((type & Compiler::StaticType::Bool) == Compiler::StaticType::Bool) {
                llvm::BasicBlock *boolBlock = llvm::BasicBlock::Create(m_llvmCtx, "bool", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), boolBlock);

                // Since the value is deallocated later, we need to create a copy
                m_builder.SetInsertPoint(boolBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *value = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                llvm::Value *stringPtr = m_builder.CreateCall(m_functions.resolve_value_boolToStringPtr(), value);
                llvm::Value *boolResult = m_builder.CreateCall(m_functions.resolve_string_pool_new(), m_builder.getInt1(true));
                m_builder.CreateCall(m_functions.resolve_string_assign(), { boolResult, stringPtr });
                m_builder.CreateBr(mergeBlock);
                results.push_back({ boolBlock, boolResult });
            }

            // String case
            if ((type & Compiler::StaticType::String) == Compiler::StaticType::String) {
                llvm::BasicBlock *stringBlock = llvm::BasicBlock::Create(m_llvmCtx, "string", m_function);
                sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), stringBlock);

                m_builder.SetInsertPoint(stringBlock);
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                llvm::Value *stringPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
                llvm::Value *stringResult = m_builder.CreateCall(m_functions.resolve_string_pool_new(), m_builder.getInt1(true));
                m_builder.CreateCall(m_functions.resolve_string_assign(), { stringResult, stringPtr });
                m_builder.CreateBr(mergeBlock);
                results.push_back({ stringBlock, stringResult });
            }

            break;
        }

        default:
            // Multiple types
            return createValue(reg);
    }

    // Default case
    m_builder.SetInsertPoint(defaultBlock);

    // All possible types are covered, mark as unreachable
    m_builder.CreateUnreachable();

    // Create phi node to merge results
    m_builder.SetInsertPoint(mergeBlock);
    llvm::PHINode *result = m_builder.CreatePHI(getType(targetType, false), results.size());

    for (auto &pair : results)
        result->addIncoming(pair.second, pair.first);

    return result;
}

llvm::Type *LLVMBuildUtils::getType(Compiler::StaticType type, bool isReturnType)
{
    switch (type) {
        case Compiler::StaticType::Void:
            return m_builder.getVoidTy();

        case Compiler::StaticType::Number:
            return m_builder.getDoubleTy();

        case Compiler::StaticType::Bool:
            return m_builder.getInt1Ty();

        case Compiler::StaticType::String:
            return m_stringPtrType->getPointerTo();

        case Compiler::StaticType::Pointer:
            return m_builder.getVoidTy()->getPointerTo();

        default:
            // Multiple types
            if (isReturnType)
                return m_valueDataType;
            else
                return m_valueDataType->getPointerTo();
    }
}

llvm::Value *LLVMBuildUtils::isNaN(llvm::Value *num)
{
    return m_builder.CreateFCmpUNO(num, num);
}

llvm::Value *LLVMBuildUtils::removeNaN(llvm::Value *num)
{
    // Replace NaN with zero
    return m_builder.CreateSelect(isNaN(num), llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0)), num);
}

void LLVMBuildUtils::createValueStore(llvm::Value *destPtr, llvm::Value *destTypePtr, LLVMRegister *reg, Compiler::StaticType destType, Compiler::StaticType targetType)
{
    llvm::Value *targetPtr = nullptr;
    const bool targetTypeIsSingle = isSingleType(targetType);

    if (targetTypeIsSingle)
        targetPtr = castValue(reg, targetType);

    auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [targetType](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == targetType; });
    const ValueType mappedType = it == TYPE_MAP.cend() ? ValueType::Number : it->first; // unknown type can be ignored
    assert(!(reg->isRawValue && it == TYPE_MAP.cend()));

    // Handle multiple type cases with runtime switch
    llvm::Value *loadedTargetType = nullptr;

    if (reg->isRawValue)
        loadedTargetType = m_builder.getInt32(static_cast<uint32_t>(mappedType));
    else {
        assert(!reg->isConst());
        llvm::Value *targetTypePtr = getValueTypePtr(reg);
        loadedTargetType = m_builder.CreateLoad(m_builder.getInt32Ty(), targetTypePtr);
    }

    llvm::Value *loadedDestType = m_builder.CreateLoad(m_builder.getInt32Ty(), destTypePtr);

    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(m_llvmCtx, "merge", m_function);
    llvm::BasicBlock *defaultBlock = llvm::BasicBlock::Create(m_llvmCtx, "default", m_function);

    llvm::SwitchInst *sw = m_builder.CreateSwitch(loadedDestType, defaultBlock, 4);

    if ((destType & Compiler::StaticType::Number) == Compiler::StaticType::Number) {
        // Writing to number
        llvm::BasicBlock *numberDestBlock = llvm::BasicBlock::Create(m_llvmCtx, "numberDest", m_function);
        sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), numberDestBlock);
        m_builder.SetInsertPoint(numberDestBlock);

        llvm::SwitchInst *targetSw = m_builder.CreateSwitch(loadedTargetType, defaultBlock, 4);

        if ((targetType & Compiler::StaticType::Number) == Compiler::StaticType::Number) {
            // Writing number to number
            llvm::BasicBlock *numberBlock = llvm::BasicBlock::Create(m_llvmCtx, "number", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), numberBlock);
            m_builder.SetInsertPoint(numberBlock);

            // Load number
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
            }

            // Write number to number directly
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            m_builder.CreateStore(targetPtr, ptr);
            m_builder.CreateBr(mergeBlock);
        }

        if ((targetType & Compiler::StaticType::Bool) == Compiler::StaticType::Bool) {
            // Writing bool to number
            llvm::BasicBlock *boolBlock = llvm::BasicBlock::Create(m_llvmCtx, "bool", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), boolBlock);
            m_builder.SetInsertPoint(boolBlock);

            // Load bool
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
            }

            // Write bool to number value directly and change type
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            m_builder.CreateStore(targetPtr, ptr);
            m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), destTypePtr);
            m_builder.CreateBr(mergeBlock);
        }

        if ((targetType & Compiler::StaticType::String) == Compiler::StaticType::String) {
            // Writing string to number
            llvm::BasicBlock *stringBlock = llvm::BasicBlock::Create(m_llvmCtx, "string", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), stringBlock);
            m_builder.SetInsertPoint(stringBlock);

            // Load string pointer
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
            }

            // Create a new string, change type and assign
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            llvm::Value *destStringPtr = m_builder.CreateCall(m_functions.resolve_string_pool_new(), m_builder.getInt1(false));

            m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), destTypePtr);
            m_builder.CreateStore(destStringPtr, ptr);
            m_builder.CreateCall(m_functions.resolve_string_assign(), { destStringPtr, targetPtr });

            m_builder.CreateBr(mergeBlock);
        }
    }

    if ((destType & Compiler::StaticType::Bool) == Compiler::StaticType::Bool) {
        // Writing to bool
        llvm::BasicBlock *boolDestBlock = llvm::BasicBlock::Create(m_llvmCtx, "boolDest", m_function);
        sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), boolDestBlock);
        m_builder.SetInsertPoint(boolDestBlock);

        llvm::SwitchInst *targetSw = m_builder.CreateSwitch(loadedTargetType, defaultBlock, 4);

        if ((targetType & Compiler::StaticType::Number) == Compiler::StaticType::Number) {
            // Writing number to bool
            llvm::BasicBlock *numberBlock = llvm::BasicBlock::Create(m_llvmCtx, "number", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), numberBlock);
            m_builder.SetInsertPoint(numberBlock);

            // Load number
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
            }

            // Write number to bool value directly and change type
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            m_builder.CreateStore(targetPtr, ptr);
            m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), destTypePtr);
            m_builder.CreateBr(mergeBlock);
        }

        if ((targetType & Compiler::StaticType::Bool) == Compiler::StaticType::Bool) {
            // Writing bool to bool
            llvm::BasicBlock *boolBlock = llvm::BasicBlock::Create(m_llvmCtx, "bool", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), boolBlock);
            m_builder.SetInsertPoint(boolBlock);

            // Load bool
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
            }

            // Write bool to bool directly
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            m_builder.CreateStore(targetPtr, ptr);
            m_builder.CreateBr(mergeBlock);
        }

        if ((targetType & Compiler::StaticType::String) == Compiler::StaticType::String) {
            // Writing string to bool
            llvm::BasicBlock *stringBlock = llvm::BasicBlock::Create(m_llvmCtx, "string", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), stringBlock);
            m_builder.SetInsertPoint(stringBlock);

            // Load string pointer
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
            }

            // Create a new string, change type and assign
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            llvm::Value *destStringPtr = m_builder.CreateCall(m_functions.resolve_string_pool_new(), m_builder.getInt1(false));

            m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), destTypePtr);
            m_builder.CreateStore(destStringPtr, ptr);
            m_builder.CreateCall(m_functions.resolve_string_assign(), { destStringPtr, targetPtr });

            m_builder.CreateBr(mergeBlock);
        }
    }

    if ((destType & Compiler::StaticType::String) == Compiler::StaticType::String) {
        // Writing to string
        llvm::BasicBlock *stringDestBlock = llvm::BasicBlock::Create(m_llvmCtx, "stringDest", m_function);
        sw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), stringDestBlock);
        m_builder.SetInsertPoint(stringDestBlock);

        llvm::SwitchInst *targetSw = m_builder.CreateSwitch(loadedTargetType, defaultBlock, 4);

        if ((targetType & Compiler::StaticType::Number) == Compiler::StaticType::Number) {
            // Writing number to string
            llvm::BasicBlock *numberBlock = llvm::BasicBlock::Create(m_llvmCtx, "number", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), numberBlock);
            m_builder.SetInsertPoint(numberBlock);

            // Load number
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
            }

            // Free the string, write the number and change type
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            llvm::Value *destStringPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
            m_builder.CreateCall(m_functions.resolve_string_pool_free(), destStringPtr);
            m_builder.CreateStore(targetPtr, ptr);
            m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)), destTypePtr);
            m_builder.CreateBr(mergeBlock);
        }

        if ((targetType & Compiler::StaticType::Bool) == Compiler::StaticType::Bool) {
            // Writing bool to string
            llvm::BasicBlock *boolBlock = llvm::BasicBlock::Create(m_llvmCtx, "bool", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), boolBlock);
            m_builder.SetInsertPoint(boolBlock);

            // Load bool
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
            }

            // Free the string, write the bool and change type
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            llvm::Value *destStringPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
            m_builder.CreateCall(m_functions.resolve_string_pool_free(), destStringPtr);
            m_builder.CreateStore(targetPtr, ptr);
            m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)), destTypePtr);
            m_builder.CreateBr(mergeBlock);
        }

        if ((targetType & Compiler::StaticType::String) == Compiler::StaticType::String) {
            // Writing string to string
            llvm::BasicBlock *stringBlock = llvm::BasicBlock::Create(m_llvmCtx, "string", m_function);
            targetSw->addCase(m_builder.getInt32(static_cast<uint32_t>(ValueType::String)), stringBlock);
            m_builder.SetInsertPoint(stringBlock);

            // Load string pointer
            if (!targetTypeIsSingle) {
                llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                targetPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
            }

            // Assign string directly
            llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, destPtr, 0);
            llvm::Value *destStringPtr = m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
            m_builder.CreateCall(m_functions.resolve_string_assign(), { destStringPtr, targetPtr });
            m_builder.CreateBr(mergeBlock);
        }
    }

    // Default case - unreachable
    m_builder.SetInsertPoint(defaultBlock);
    m_builder.CreateUnreachable();

    m_builder.SetInsertPoint(mergeBlock);
}

void LLVMBuildUtils::createValueStore(llvm::Value *destPtr, llvm::Value *destTypePtr, LLVMRegister *reg, Compiler::StaticType targetType)
{
    // Same as createValueStore(), but the destination type is unknown at compile time
    createValueStore(destPtr, destTypePtr, reg, Compiler::StaticType::Unknown, targetType);
}

llvm::Value *LLVMBuildUtils::getValueTypePtr(llvm::Value *value)
{
    return m_builder.CreateStructGEP(m_valueDataType, value, 1);
}

llvm::Value *LLVMBuildUtils::getValueTypePtr(LLVMRegister *reg)
{
    if (reg->typeVar)
        return reg->typeVar;
    else
        return getValueTypePtr(reg->value);
}

llvm::Value *LLVMBuildUtils::getListSize(const LLVMListPtr &listPtr)
{
    return m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.size ? listPtr.size : listPtr.sizePtr);
}

llvm::Value *LLVMBuildUtils::getListItem(const LLVMListPtr &listPtr, llvm::Value *index)
{
    return m_builder.CreateGEP(m_valueDataType, getListDataPtr(listPtr), index);
}

llvm::Value *LLVMBuildUtils::getListItemIndex(const LLVMListPtr &listPtr, Compiler::StaticType listType, LLVMRegister *item)
{
    llvm::Value *size = getListSize(listPtr);
    llvm::BasicBlock *condBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
    llvm::BasicBlock *cmpIfBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
    llvm::BasicBlock *cmpElseBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
    llvm::BasicBlock *notFoundBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);

    // index = 0
    llvm::Value *index = addAlloca(m_builder.getInt64Ty());
    m_builder.CreateStore(m_builder.getInt64(0), index);
    m_builder.CreateBr(condBlock);

    // while (index < size)
    m_builder.SetInsertPoint(condBlock);
    llvm::Value *cond = m_builder.CreateICmpULT(m_builder.CreateLoad(m_builder.getInt64Ty(), index), size);
    m_builder.CreateCondBr(cond, bodyBlock, notFoundBlock);

    // if (list[index] == item)
    m_builder.SetInsertPoint(bodyBlock);
    LLVMRegister currentItem(listType);
    currentItem.isRawValue = false;
    currentItem.value = getListItem(listPtr, m_builder.CreateLoad(m_builder.getInt64Ty(), index));
    llvm::Value *cmp = createComparison(&currentItem, item, Comparison::EQ);
    m_builder.CreateCondBr(cmp, cmpIfBlock, cmpElseBlock);

    // goto nextBlock
    m_builder.SetInsertPoint(cmpIfBlock);
    m_builder.CreateBr(nextBlock);

    // else index++
    m_builder.SetInsertPoint(cmpElseBlock);
    m_builder.CreateStore(m_builder.CreateAdd(m_builder.CreateLoad(m_builder.getInt64Ty(), index), m_builder.getInt64(1)), index);
    m_builder.CreateBr(condBlock);

    // notFoundBlock:
    // index = -1
    // goto nextBlock
    m_builder.SetInsertPoint(notFoundBlock);
    m_builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt64Ty(m_llvmCtx), -1, true), index);
    m_builder.CreateBr(nextBlock);

    // nextBlock:
    m_builder.SetInsertPoint(nextBlock);

    return m_builder.CreateLoad(m_builder.getInt64Ty(), index);
}

llvm::Value *LLVMBuildUtils::createValue(LLVMRegister *reg)
{
    if (reg->isConst()) {
        // Create a constant ValueData instance and store it
        llvm::Constant *value = castConstValue(reg->constValue(), TYPE_MAP[reg->constValue().type()]);
        llvm::Value *ret = addAlloca(m_valueDataType);

        switch (reg->constValue().type()) {
            case ValueType::Number:
                value = llvm::ConstantExpr::getBitCast(value, m_valueDataType->getElementType(0));
                break;

            case ValueType::Bool:
                // Assuming union type is int64
                value = m_builder.getInt64(reg->constValue().toBool());
                break;

            case ValueType::String:
            case ValueType::Pointer:
                value = llvm::ConstantExpr::getPtrToInt(value, m_valueDataType->getElementType(0));
                break;

            default:
                assert(false);
                break;
        }

        llvm::Constant *type = m_builder.getInt32(static_cast<uint32_t>(reg->constValue().type()));
        llvm::Constant *padding = m_builder.getInt32(0);
        llvm::Constant *constValue = llvm::ConstantStruct::get(m_valueDataType, { value, type, padding });
        m_builder.CreateStore(constValue, ret);

        return ret;
    } else if (reg->isRawValue) {
        llvm::Value *value = castRawValue(reg, reg->type());
        llvm::Value *ret = addAlloca(m_valueDataType);

        // Store value
        llvm::Value *valueField = m_builder.CreateStructGEP(m_valueDataType, ret, 0);
        m_builder.CreateStore(value, valueField);

        auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [&reg](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == reg->type(); });

        if (it == TYPE_MAP.end()) {
            assert(false);
            return nullptr;
        }

        // Store type
        llvm::Value *typeField = getValueTypePtr(ret);
        ValueType type = it->first;
        m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(type)), typeField);

        return ret;
    } else
        return reg->value;
}

llvm::Value *LLVMBuildUtils::createNewValue(LLVMRegister *reg)
{
    // Same as createValue(), but creates a copy of the contents
    // NOTE: It is the caller's responsibility to free the value.
    llvm::Value *value = createValue(reg);
    llvm::Value *ret = addAlloca(m_valueDataType);
    m_builder.CreateCall(m_functions.resolve_value_init(), { ret });
    m_builder.CreateCall(m_functions.resolve_value_assign_copy(), { ret, value });
    return ret;
}

llvm::Value *LLVMBuildUtils::createComparison(LLVMRegister *arg1, LLVMRegister *arg2, Comparison type)
{
    auto type1 = arg1->type();
    auto type2 = arg2->type();

    if (arg1->isConst() && arg2->isConst()) {
        // If both operands are constant, perform the comparison at compile time
        bool result = false;

        switch (type) {
            case Comparison::EQ:
                result = arg1->constValue() == arg2->constValue();
                break;

            case Comparison::GT:
                result = arg1->constValue() > arg2->constValue();
                break;

            case Comparison::LT:
                result = arg1->constValue() < arg2->constValue();
                break;

            default:
                assert(false);
                return nullptr;
        }

        return m_builder.getInt1(result);
    } else {
        // Optimize comparison of constant with number/bool
        if (arg1->isConst() && arg1->constValue().isValidNumber() && (type2 == Compiler::StaticType::Number || type2 == Compiler::StaticType::Bool))
            type1 = Compiler::StaticType::Number;

        if (arg2->isConst() && arg2->constValue().isValidNumber() && (type1 == Compiler::StaticType::Number || type1 == Compiler::StaticType::Bool))
            type2 = Compiler::StaticType::Number;

        // Optimize number and bool comparison
        int optNumberBool = 0;

        if (type1 == Compiler::StaticType::Number && type2 == Compiler::StaticType::Bool) {
            type2 = Compiler::StaticType::Number;
            optNumberBool = 2; // operand 2 was bool
        }

        if (type1 == Compiler::StaticType::Bool && type2 == Compiler::StaticType::Number) {
            type1 = Compiler::StaticType::Number;
            optNumberBool = 1; // operand 1 was bool
        }

        // Optimize number and string constant comparison
        // TODO: GT and LT comparison can be optimized here (e. g. by checking the string constant characters and comparing with numbers and .+-e)
        if (type == Comparison::EQ) {
            if ((type1 == Compiler::StaticType::Number && type2 == Compiler::StaticType::String && arg2->isConst() && !arg2->constValue().isValidNumber()) ||
                (type1 == Compiler::StaticType::String && type2 == Compiler::StaticType::Number && arg1->isConst() && !arg1->constValue().isValidNumber()))
                return m_builder.getInt1(false);
        }

        if (type1 != type2 || !isSingleType(type1) || !isSingleType(type2)) {
            // If the types are different or at least one of them
            // is unknown, we must use value functions
            llvm::Value *value1 = createValue(arg1);
            llvm::Value *value2 = createValue(arg2);

            switch (type) {
                case Comparison::EQ:
                    return m_builder.CreateCall(m_functions.resolve_value_equals(), { value1, value2 });

                case Comparison::GT:
                    return m_builder.CreateCall(m_functions.resolve_value_greater(), { value1, value2 });

                case Comparison::LT:
                    return m_builder.CreateCall(m_functions.resolve_value_lower(), { value1, value2 });

                default:
                    assert(false);
                    return nullptr;
            }
        } else {
            // Compare raw values
            llvm::Value *value1 = castValue(arg1, type1);
            llvm::Value *value2 = castValue(arg2, type2);
            assert(type1 == type2);

            switch (type1) {
                case Compiler::StaticType::Number: {
                    // Compare two numbers
                    switch (type) {
                        case Comparison::EQ: {
                            llvm::Value *nan = m_builder.CreateAnd(isNaN(value1), isNaN(value2)); // NaN == NaN
                            llvm::Value *cmp = m_builder.CreateFCmpOEQ(value1, value2);
                            return m_builder.CreateSelect(nan, m_builder.getInt1(true), cmp);
                        }

                        case Comparison::GT: {
                            llvm::Value *bothNan = m_builder.CreateAnd(isNaN(value1), isNaN(value2)); // NaN == NaN
                            llvm::Value *cmp = m_builder.CreateFCmpOGT(value1, value2);
                            llvm::Value *nan;
                            llvm::Value *nanCmp;

                            if (optNumberBool == 1) {
                                nan = isNaN(value2);
                                nanCmp = castValue(arg1, Compiler::StaticType::Bool);
                            } else if (optNumberBool == 2) {
                                nan = isNaN(value1);
                                nanCmp = m_builder.CreateNot(castValue(arg2, Compiler::StaticType::Bool));
                            } else {
                                nan = isNaN(value1);
                                nanCmp = m_builder.CreateFCmpUGT(value1, value2);
                            }

                            return m_builder.CreateAnd(m_builder.CreateNot(bothNan), m_builder.CreateSelect(nan, nanCmp, cmp));
                        }

                        case Comparison::LT: {
                            llvm::Value *bothNan = m_builder.CreateAnd(isNaN(value1), isNaN(value2)); // NaN == NaN
                            llvm::Value *cmp = m_builder.CreateFCmpOLT(value1, value2);
                            llvm::Value *nan;
                            llvm::Value *nanCmp;

                            if (optNumberBool == 1) {
                                nan = isNaN(value2);
                                nanCmp = m_builder.CreateNot(castValue(arg1, Compiler::StaticType::Bool));
                            } else if (optNumberBool == 2) {
                                nan = isNaN(value1);
                                nanCmp = castValue(arg2, Compiler::StaticType::Bool);
                            } else {
                                nan = isNaN(value2);
                                nanCmp = m_builder.CreateFCmpULT(value1, value2);
                            }

                            return m_builder.CreateAnd(m_builder.CreateNot(bothNan), m_builder.CreateSelect(nan, nanCmp, cmp));
                        }

                        default:
                            assert(false);
                            return nullptr;
                    }
                }

                case Compiler::StaticType::Bool:
                    // Compare two booleans
                    switch (type) {
                        case Comparison::EQ:
                            return m_builder.CreateICmpEQ(value1, value2);

                        case Comparison::GT:
                            // value1 && !value2
                            return m_builder.CreateAnd(value1, m_builder.CreateNot(value2));

                        case Comparison::LT:
                            // value2 && !value1
                            return m_builder.CreateAnd(value2, m_builder.CreateNot(value1));

                        default:
                            assert(false);
                            return nullptr;
                    }

                case Compiler::StaticType::String: {
                    // Compare two strings
                    llvm::Value *cmpRet = m_builder.CreateCall(m_functions.resolve_string_compare_case_insensitive(), { value1, value2 });

                    switch (type) {
                        case Comparison::EQ:
                            return m_builder.CreateICmpEQ(cmpRet, m_builder.getInt32(0));

                        case Comparison::GT:
                            return m_builder.CreateICmpSGT(cmpRet, m_builder.getInt32(0));

                        case Comparison::LT:
                            return m_builder.CreateICmpSLT(cmpRet, m_builder.getInt32(0));

                        default:
                            assert(false);
                            return nullptr;
                    }
                }

                default:
                    assert(false);
                    return nullptr;
            }
        }
    }
}

llvm::Value *LLVMBuildUtils::createStringComparison(LLVMRegister *arg1, LLVMRegister *arg2, bool caseSensitive)
{
    auto type1 = arg1->type();
    auto type2 = arg2->type();

    if (arg1->isConst() && arg2->isConst()) {
        // If both operands are constant, perform the comparison at compile time
        StringPtr *str1 = value_toStringPtr(&arg1->constValue().data());
        StringPtr *str2 = value_toStringPtr(&arg2->constValue().data());
        bool result;

        if (caseSensitive)
            result = string_compare_case_sensitive(str1, str2) == 0;
        else {
            result = string_compare_case_insensitive(str1, str2) == 0;
        }

        string_pool_free(str1);
        string_pool_free(str2);
        return m_builder.getInt1(result);
    } else {
        // Optimize number and string constant comparison
        // TODO: Optimize bool and string constant comparison (in compare() as well)
        if ((type1 == Compiler::StaticType::Number && type2 == Compiler::StaticType::String && arg2->isConst() && !arg2->constValue().isValidNumber()) ||
            (type1 == Compiler::StaticType::String && type2 == Compiler::StaticType::Number && arg1->isConst() && !arg1->constValue().isValidNumber()))
            return m_builder.getInt1(false);

        // Explicitly cast to string
        llvm::Value *string1 = castValue(arg1, Compiler::StaticType::String);
        llvm::Value *string2 = castValue(arg2, Compiler::StaticType::String);
        llvm::Value *cmp = m_builder.CreateCall(caseSensitive ? m_functions.resolve_string_compare_case_sensitive() : m_functions.resolve_string_compare_case_insensitive(), { string1, string2 });
        return m_builder.CreateICmpEQ(cmp, m_builder.getInt32(0));
    }
}

void LLVMBuildUtils::createSuspend()
{
    if (m_coroutine) {
        assert(!m_warp);
        llvm::BasicBlock *suspendBranch, *nextBranch;

        if (m_warpArg) {
            suspendBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
            nextBranch = llvm::BasicBlock::Create(m_llvmCtx, "", m_function);
            m_builder.CreateCondBr(m_warpArg, nextBranch, suspendBranch);
            m_builder.SetInsertPoint(suspendBranch);
        }

        syncVariables();
        m_coroutine->createSuspend();
        reloadVariables();
        reloadLists();

        if (m_warpArg) {
            m_builder.CreateBr(nextBranch);
            m_builder.SetInsertPoint(nextBranch);
        }
    }
}

void LLVMBuildUtils::initTypes()
{
    m_valueDataType = m_ctx->valueDataType();
    m_stringPtrType = m_ctx->stringPtrType();
}

void LLVMBuildUtils::createVariableMap()
{
    if (!m_target)
        return;

    // Map variable pointers to variable data array indices
    const auto &variables = m_target->variables();
    ValueData **variableData = m_target->variableData();
    const size_t len = variables.size();
    m_targetVariableMap.clear();
    m_targetVariableMap.reserve(len);

    size_t i, j;

    for (i = 0; i < len; i++) {
        Variable *var = variables[i].get();

        // Find the data for this variable
        for (j = 0; j < len; j++) {
            if (variableData[j] == &var->valuePtr()->data())
                break;
        }

        if (j < len)
            m_targetVariableMap[var] = j;
        else
            assert(false);
    }
}

void LLVMBuildUtils::createListMap()
{
    if (!m_target)
        return;

    // Map list pointers to list array indices
    const auto &lists = m_target->lists();
    List **listData = m_target->listData();
    const size_t len = lists.size();
    m_targetListMap.clear();
    m_targetListMap.reserve(len);

    size_t i, j;

    for (i = 0; i < len; i++) {
        List *list = lists[i].get();

        // Find this list
        for (j = 0; j < len; j++) {
            if (listData[j] == list)
                break;
        }

        if (j < len)
            m_targetListMap[list] = j;
        else
            assert(false);
    }
}

llvm::Value *LLVMBuildUtils::castRawValue(LLVMRegister *reg, Compiler::StaticType targetType)
{
    if (reg->type() == targetType)
        return reg->value;

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type()) {
                case Compiler::StaticType::Bool:
                    // Cast bool to double
                    return m_builder.CreateUIToFP(reg->value, m_builder.getDoubleTy());

                case Compiler::StaticType::String: {
                    // Convert string to double
                    return m_builder.CreateCall(m_functions.resolve_value_stringToDouble(), reg->value);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Bool:
            switch (reg->type()) {
                case Compiler::StaticType::Number:
                    // Cast double to bool (true if != 0)
                    return m_builder.CreateFCmpONE(reg->value, llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0)));

                case Compiler::StaticType::String:
                    // Convert string to bool
                    return m_builder.CreateCall(m_functions.resolve_value_stringToBool(), reg->value);

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::String:
            switch (reg->type()) {
                case Compiler::StaticType::Number: {
                    // Convert double to string
                    llvm::Value *ptr = m_builder.CreateCall(m_functions.resolve_value_doubleToStringPtr(), reg->value);
                    freeStringLater(ptr);
                    return ptr;
                }

                case Compiler::StaticType::Bool: {
                    // Convert bool to string
                    llvm::Value *ptr = m_builder.CreateCall(m_functions.resolve_value_boolToStringPtr(), reg->value);
                    // NOTE: Dot not deallocate later
                    return ptr;
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Unknown:
            return createValue(reg);

        default:
            assert(false);
            return nullptr;
    }
}

llvm::Constant *LLVMBuildUtils::castConstValue(const Value &value, Compiler::StaticType targetType)
{
    switch (targetType) {
        case Compiler::StaticType::Number: {
            const double nan = std::numeric_limits<double>::quiet_NaN();
            return llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(value.isNaN() ? nan : value.toDouble()));
        }

        case Compiler::StaticType::Bool:
            return m_builder.getInt1(value.toBool());

        case Compiler::StaticType::String: {
            std::u16string str = value.toUtf16();

            // Create a constant array for the string
            std::vector<llvm::Constant *> elements;
            for (char16_t ch : str)
                elements.push_back(m_builder.getInt16(ch));

            elements.push_back(m_builder.getInt16(0)); // null terminator

            llvm::ArrayType *arrayType = llvm::ArrayType::get(m_builder.getInt16Ty(), elements.size());
            llvm::Constant *constArray = llvm::ConstantArray::get(arrayType, elements);

            llvm::Module &module = *m_ctx->module();

            llvm::Constant *globalStr = new llvm::GlobalVariable(module, arrayType, true, llvm::GlobalValue::PrivateLinkage, constArray, "string");
            llvm::Constant *stringStruct = llvm::ConstantStruct::get(m_stringPtrType, { globalStr, m_builder.getInt64(str.size()), m_builder.getInt64(str.size() + 1) });
            return new llvm::GlobalVariable(module, m_stringPtrType, true, llvm::GlobalValue::PrivateLinkage, stringStruct, "stringPtr");
        }

        case Compiler::StaticType::Pointer: {
            llvm::Constant *addr = m_builder.getInt64((uintptr_t)value.toPointer());
            return llvm::ConstantExpr::getIntToPtr(addr, m_builder.getVoidTy()->getPointerTo());
        }

        default:
            assert(false);
            return nullptr;
    }
}

void LLVMBuildUtils::createValueCopy(llvm::Value *source, llvm::Value *target)
{
    // NOTE: This doesn't copy strings, but only the pointers
    copyStructField(source, target, 0, m_valueDataType, m_builder.getInt64Ty()); // value
    copyStructField(source, target, 1, m_valueDataType, m_builder.getInt32Ty()); // type
    /* 2: padding */
}

void LLVMBuildUtils::copyStructField(llvm::Value *source, llvm::Value *target, int index, llvm::StructType *structType, llvm::Type *fieldType)
{
    llvm::Value *sourceField = m_builder.CreateStructGEP(structType, source, index);
    llvm::Value *targetField = m_builder.CreateStructGEP(structType, target, index);
    m_builder.CreateStore(m_builder.CreateLoad(fieldType, sourceField), targetField);
}

llvm::Value *LLVMBuildUtils::getVariablePtr(llvm::Value *targetVariables, Variable *variable)
{
    if (!m_target->isStage() && variable->target() == m_target) {
        // If this is a local sprite variable, use the variable array at runtime (for clones)
        assert(m_targetVariableMap.find(variable) != m_targetVariableMap.cend());
        const size_t index = m_targetVariableMap[variable];
        llvm::Value *ptr = m_builder.CreateGEP(m_valueDataType->getPointerTo(), targetVariables, m_builder.getInt64(index));
        return m_builder.CreateLoad(m_valueDataType->getPointerTo(), ptr);
    }

    // Otherwise create a raw pointer at compile time
    llvm::Value *addr = m_builder.getInt64((uintptr_t)&variable->value().data());
    return m_builder.CreateIntToPtr(addr, m_valueDataType->getPointerTo());
}

llvm::Value *LLVMBuildUtils::getListPtr(llvm::Value *targetLists, List *list)
{
    if (!m_target->isStage() && list->target() == m_target) {
        // If this is a local sprite list, use the list array at runtime (for clones)
        assert(m_targetListMap.find(list) != m_targetListMap.cend());
        const size_t index = m_targetListMap[list];
        auto pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(m_llvmCtx), 0);
        llvm::Value *ptr = m_builder.CreateGEP(pointerType, targetLists, m_builder.getInt64(index));
        return m_builder.CreateLoad(pointerType, ptr);
    }

    // Otherwise create a raw pointer at compile time
    llvm::Value *addr = m_builder.getInt64((uintptr_t)list);
    return m_builder.CreateIntToPtr(addr, m_valueDataType->getPointerTo());
}

llvm::Value *LLVMBuildUtils::getListDataPtr(const LLVMListPtr &listPtr)
{
    return m_builder.CreateLoad(m_valueDataType->getPointerTo(), m_builder.CreateLoad(m_valueDataType->getPointerTo()->getPointerTo(), listPtr.dataPtr));
}
