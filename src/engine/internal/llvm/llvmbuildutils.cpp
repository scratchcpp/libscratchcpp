// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/target.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>

#include "llvmbuildutils.h"
#include "llvmfunctions.h"
#include "llvmcompilercontext.h"
#include "llvmregister.h"

using namespace libscratchcpp;

static std::unordered_map<ValueType, Compiler::StaticType> TYPE_MAP = {
    { ValueType::Number, Compiler::StaticType::Number },
    { ValueType::Bool, Compiler::StaticType::Bool },
    { ValueType::String, Compiler::StaticType::String },
    { ValueType::Pointer, Compiler::StaticType::Pointer }
};

LLVMBuildUtils::LLVMBuildUtils(LLVMCompilerContext *ctx, llvm::IRBuilder<> &builder) :
    m_ctx(ctx),
    m_llvmCtx(*ctx->llvmCtx()),
    m_builder(builder),
    m_functions(ctx, &builder),
    m_target(ctx->target())
{
    initTypes();
    createVariableMap();
    createListMap();
}

void LLVMBuildUtils::init(llvm::Function *function, llvm::Value *targetVariables, llvm::Value *targetLists)
{
    m_function = function;
    m_targetVariables = targetVariables;
    m_targetLists = targetLists;
    m_stringHeap.clear();
    pushScopeLevel();

    // Create variable pointers
    for (auto &[var, varPtr] : m_variablePtrs) {
        llvm::Value *ptr = getVariablePtr(targetVariables, var);

        // Direct access
        varPtr.heapPtr = ptr;

        // All variables are currently created on the stack and synced later (seems to be faster)
        // NOTE: Strings are NOT copied, only the pointer is copied
        // TODO: Restore this feature
        // varPtr.stackPtr = m_builder.CreateAlloca(m_valueDataType);
        varPtr.stackPtr = varPtr.heapPtr;
        varPtr.onStack = false;

        // If there are no write operations outside loops, initialize the stack variable now
        /*Variable *variable = var;
        // TODO: Loop scope was used here, replace it with some "inside loop" flag if needed
        auto it = std::find_if(m_variableInstructions.begin(), m_variableInstructions.end(), [variable](const LLVMInstruction *ins) {
            return ins->type == LLVMInstruction::Type::WriteVariable && ins->workVariable == variable && !ins->loopScope;
        });

        if (it == m_variableInstructions.end()) {
            createValueCopy(ptr, varPtr.stackPtr);
            varPtr.onStack = true;
        } else
            varPtr.onStack = false; // use heap before the first assignment
        */
    }

    // Create list pointers
    for (auto &[list, listPtr] : m_listPtrs) {
        listPtr.ptr = getListPtr(targetLists, list);

        listPtr.dataPtr = m_builder.CreateAlloca(m_valueDataType->getPointerTo()->getPointerTo());
        m_builder.CreateStore(m_builder.CreateCall(m_functions.resolve_list_data_ptr(), listPtr.ptr), listPtr.dataPtr);

        listPtr.sizePtr = m_builder.CreateCall(m_functions.resolve_list_size_ptr(), listPtr.ptr);
        listPtr.allocatedSizePtr = m_builder.CreateCall(m_functions.resolve_list_alloc_size_ptr(), listPtr.ptr);
    }
}

void LLVMBuildUtils::end()
{
    assert(m_stringHeap.size() == 1);
    freeScopeHeap();
}

llvm::IRBuilder<> &LLVMBuildUtils::builder()
{
    return m_builder;
}

LLVMFunctions &LLVMBuildUtils::functions()
{
    return m_functions;
}

llvm::Value *LLVMBuildUtils::targetVariables()
{
    return m_targetVariables;
}

llvm::Value *LLVMBuildUtils::targetLists()
{
    return m_targetLists;
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

void LLVMBuildUtils::syncVariables(llvm::Value *targetVariables)
{
    // Copy stack variables to the actual variables
    for (auto &[var, varPtr] : m_variablePtrs) {
        if (varPtr.onStack && varPtr.changed)
            createValueCopy(varPtr.stackPtr, getVariablePtr(targetVariables, var));

        varPtr.changed = false;
    }
}

void LLVMBuildUtils::reloadVariables(llvm::Value *targetVariables)
{
    // Reset variables to use heap
    for (auto &[var, varPtr] : m_variablePtrs) {
        varPtr.onStack = false;
        varPtr.changed = false;
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

Compiler::StaticType LLVMBuildUtils::optimizeRegisterType(LLVMRegister *reg)
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
        if (targetType == Compiler::StaticType::Unknown)
            return createValue(reg);
        else
            return castConstValue(reg->constValue(), targetType);
    }

    if (reg->isRawValue)
        return castRawValue(reg, targetType);

    assert(reg->type() != Compiler::StaticType::Void && targetType != Compiler::StaticType::Void);

    switch (targetType) {
        case Compiler::StaticType::Number:
            switch (reg->type()) {
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

                case Compiler::StaticType::String:
                case Compiler::StaticType::Unknown: {
                    // Convert to double
                    return m_builder.CreateCall(m_functions.resolve_value_toDouble(), reg->value);
                }

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::Bool:
            switch (reg->type()) {
                case Compiler::StaticType::Number: {
                    // True if != 0
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    llvm::Value *numberValue = m_builder.CreateLoad(m_builder.getDoubleTy(), ptr);
                    return m_builder.CreateFCmpONE(numberValue, llvm::ConstantFP::get(m_llvmCtx, llvm::APFloat(0.0)));
                }

                case Compiler::StaticType::Bool: {
                    // Read boolean directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(m_builder.getInt1Ty(), ptr);
                }

                case Compiler::StaticType::String:
                case Compiler::StaticType::Unknown:
                    // Convert to bool
                    return m_builder.CreateCall(m_functions.resolve_value_toBool(), reg->value);

                default:
                    assert(false);
                    return nullptr;
            }

        case Compiler::StaticType::String:
            switch (reg->type()) {
                case Compiler::StaticType::Number:
                case Compiler::StaticType::Bool:
                case Compiler::StaticType::Unknown: {
                    // Cast to string
                    // TODO: Use value_stringToDouble() and value_stringToBool()
                    llvm::Value *ptr = m_builder.CreateCall(m_functions.resolve_value_toStringPtr(), reg->value);
                    freeStringLater(ptr);
                    return ptr;
                }

                case Compiler::StaticType::String: {
                    // Read string pointer directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, reg->value, 0);
                    return m_builder.CreateLoad(m_stringPtrType->getPointerTo(), ptr);
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

llvm::Type *LLVMBuildUtils::getType(Compiler::StaticType type)
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

        case Compiler::StaticType::Unknown:
            return m_valueDataType->getPointerTo();

        default:
            assert(false);
            return nullptr;
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

void LLVMBuildUtils::createValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType)
{
    llvm::Value *converted = nullptr;

    if (sourceType != Compiler::StaticType::Unknown)
        converted = castValue(reg, sourceType);

    auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [sourceType](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == sourceType; });
    const ValueType mappedType = it == TYPE_MAP.cend() ? ValueType::Number : it->first; // unknown type can be ignored

    switch (sourceType) {
        case Compiler::StaticType::Number:
            switch (targetType) {
                case Compiler::StaticType::Number: {
                    // Write number to number directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    m_builder.CreateStore(converted, ptr);
                    break;
                }

                case Compiler::StaticType::Bool: {
                    // Write number to bool value directly and change type
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    llvm::Value *typePtr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 1);
                    m_builder.CreateStore(converted, ptr);
                    m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(mappedType)), typePtr);
                    break;
                }

                default:
                    m_builder.CreateCall(m_functions.resolve_value_assign_double(), { targetPtr, converted });
                    break;
            }

            break;

        case Compiler::StaticType::Bool:
            switch (targetType) {
                case Compiler::StaticType::Number: {
                    // Write bool to number value directly and change type
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    m_builder.CreateStore(converted, ptr);
                    llvm::Value *typePtr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 1);
                    m_builder.CreateStore(converted, ptr);
                    m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(mappedType)), typePtr);
                    break;
                }

                case Compiler::StaticType::Bool: {
                    // Write bool to bool directly
                    llvm::Value *ptr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 0);
                    m_builder.CreateStore(converted, ptr);
                    break;
                }

                default:
                    m_builder.CreateCall(m_functions.resolve_value_assign_bool(), { targetPtr, converted });
                    break;
            }

            break;

        case Compiler::StaticType::String:
            m_builder.CreateCall(m_functions.resolve_value_assign_stringPtr(), { targetPtr, converted });
            break;

        case Compiler::StaticType::Unknown:
            m_builder.CreateCall(m_functions.resolve_value_assign_copy(), { targetPtr, reg->value });
            break;

        default:
            assert(false);
            break;
    }
}

void LLVMBuildUtils::createReusedValueStore(LLVMRegister *reg, llvm::Value *targetPtr, Compiler::StaticType sourceType, Compiler::StaticType targetType)
{
    // Same as createValueStore(), but ensures that type is updated
    createValueStore(reg, targetPtr, sourceType, targetType);

    auto it = std::find_if(TYPE_MAP.begin(), TYPE_MAP.end(), [sourceType](const std::pair<ValueType, Compiler::StaticType> &pair) { return pair.second == sourceType; });
    const ValueType mappedType = it == TYPE_MAP.cend() ? ValueType::Number : it->first; // unknown type can be ignored

    if ((sourceType == Compiler::StaticType::Number || sourceType == Compiler::StaticType::Bool) && sourceType == targetType) {
        // Update type when writing number to number and bool to bool
        llvm::Value *typePtr = m_builder.CreateStructGEP(m_valueDataType, targetPtr, 1);
        m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(mappedType)), typePtr);
    }
}

llvm::Value *LLVMBuildUtils::getListItem(const LLVMListPtr &listPtr, llvm::Value *index)
{
    return m_builder.CreateGEP(m_valueDataType, getListDataPtr(listPtr), index);
}

llvm::Value *LLVMBuildUtils::getListItemIndex(const LLVMListPtr &listPtr, Compiler::StaticType listType, LLVMRegister *item)
{
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
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
        llvm::Value *typeField = m_builder.CreateStructGEP(m_valueDataType, ret, 1);
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

        if (type1 != type2 || type1 == Compiler::StaticType::Unknown || type2 == Compiler::StaticType::Unknown) {
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
