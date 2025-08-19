// SPDX-License-Identifier: Apache-2.0

#include "lists.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"
#include "../llvmconstantregister.h"
#include "../llvmcompilercontext.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

ProcessResult Lists::process(LLVMInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case LLVMInstruction::Type::ClearList:
            ret.next = buildClearList(ins);
            break;

        case LLVMInstruction::Type::RemoveListItem:
            ret.next = buildRemoveListItem(ins);
            break;

        case LLVMInstruction::Type::AppendToList:
            ret.next = buildAppendToList(ins);
            break;

        case LLVMInstruction::Type::InsertToList:
            ret.next = buildInsertToList(ins);
            break;

        case LLVMInstruction::Type::ListReplace:
            ret.next = buildListReplace(ins);
            break;

        case LLVMInstruction::Type::GetListContents:
            ret.next = buildGetListContents(ins);
            break;

        case LLVMInstruction::Type::GetListItem:
            ret.next = buildGetListItem(ins);
            break;

        case LLVMInstruction::Type::GetListSize:
            ret.next = buildGetListSize(ins);
            break;

        case LLVMInstruction::Type::GetListItemIndex:
            ret.next = buildGetListItemIndex(ins);
            break;

        case LLVMInstruction::Type::ListContainsItem:
            ret.next = buildListContainsItem(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

LLVMInstruction *Lists::buildClearList(LLVMInstruction *ins)
{
    if (ins->targetType != Compiler::StaticType::Void) { // do not clear a list that is already empty
        assert(ins->args.size() == 0);
        LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);
        m_builder.CreateCall(m_utils.functions().resolve_list_clear(), listPtr.ptr);

        if (listPtr.size) {
            // Update size
            m_builder.CreateStore(m_builder.getInt64(0), listPtr.size);
        }

        if (listPtr.hasNumber && listPtr.hasBool && listPtr.hasString) {
            // Reset type info
            m_builder.CreateStore(m_builder.getInt1(false), listPtr.hasNumber);
            m_builder.CreateStore(m_builder.getInt1(false), listPtr.hasBool);
            m_builder.CreateStore(m_builder.getInt1(false), listPtr.hasString);
        }
    }

    return ins->next;
}

LLVMInstruction *Lists::buildRemoveListItem(LLVMInstruction *ins)
{
    // No-op in empty lists
    if (ins->targetType == Compiler::StaticType::Void)
        return ins->next;

    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];

    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    // Range check
    llvm::Value *indexDouble = m_utils.castValue(arg.second, arg.first);
    llvm::Value *indexInt = getIndex(listPtr, indexDouble);
    llvm::Value *inRange = createSizeRangeCheck(listPtr, indexInt, "removeListItem.indexInRange");

    llvm::BasicBlock *removeBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(inRange, removeBlock, nextBlock);

    // Remove
    m_builder.SetInsertPoint(removeBlock);
    m_builder.CreateCall(m_utils.functions().resolve_list_remove(), { listPtr.ptr, indexInt });

    if (listPtr.size) {
        // Update size
        llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.size);
        size = m_builder.CreateSub(size, m_builder.getInt64(1));
        m_builder.CreateStore(size, listPtr.size);
    }

    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);
    return ins->next;
}

LLVMInstruction *Lists::buildAppendToList(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    Compiler::StaticType type = m_utils.optimizeRegisterType(arg.second);
    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    // Check if enough space is allocated
    llvm::Value *allocatedSize = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.allocatedSizePtr);
    llvm::Value *size = m_utils.getListSize(listPtr);
    llvm::Value *isAllocated = m_builder.CreateICmpUGT(allocatedSize, size);
    llvm::BasicBlock *ifBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(isAllocated, ifBlock, elseBlock);

    // If there's enough space, use the allocated memory
    m_builder.SetInsertPoint(ifBlock);
    llvm::Value *itemPtr = m_utils.getListItem(listPtr, size);
    m_utils.createValueStore(itemPtr, m_utils.getValueTypePtr(itemPtr), arg.second, type);
    m_builder.CreateStore(m_builder.CreateAdd(size, m_builder.getInt64(1)), listPtr.sizePtr); // update size stored in *sizePtr
    m_builder.CreateBr(nextBlock);

    // Otherwise call appendEmpty()
    m_builder.SetInsertPoint(elseBlock);
    itemPtr = m_builder.CreateCall(m_utils.functions().resolve_list_append_empty(), listPtr.ptr);
    m_utils.createValueStore(itemPtr, m_utils.getValueTypePtr(itemPtr), arg.second, type);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);

    if (listPtr.size) {
        // Update local size
        llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.size);
        size = m_builder.CreateAdd(size, m_builder.getInt64(1));
        m_builder.CreateStore(size, listPtr.size);
    }

    createListTypeUpdate(listPtr, arg.second, type);
    return ins->next;
}

LLVMInstruction *Lists::buildInsertToList(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(ins->args.size() == 2);
    const auto &indexArg = ins->args[0];
    const auto &valueArg = ins->args[1];
    Compiler::StaticType type = m_utils.optimizeRegisterType(valueArg.second);
    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    // Range check
    llvm::Value *indexDouble = m_utils.castValue(indexArg.second, indexArg.first);
    llvm::Value *indexInt = getIndex(listPtr, indexDouble);
    llvm::Value *inRange = createSizeRangeCheck(listPtr, indexInt, "insertToList.indexInRange", true);

    llvm::BasicBlock *insertBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(inRange, insertBlock, nextBlock);

    // Insert
    m_builder.SetInsertPoint(insertBlock);
    llvm::Value *itemPtr = m_builder.CreateCall(m_utils.functions().resolve_list_insert_empty(), { listPtr.ptr, indexInt });
    m_utils.createValueStore(itemPtr, m_utils.getValueTypePtr(itemPtr), valueArg.second, type);

    if (listPtr.size) {
        // Update size
        llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.size);
        size = m_builder.CreateAdd(size, m_builder.getInt64(1));
        m_builder.CreateStore(size, listPtr.size);
    }

    createListTypeUpdate(listPtr, valueArg.second, type);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);
    return ins->next;
}

LLVMInstruction *Lists::buildListReplace(LLVMInstruction *ins)
{
    // No-op in empty lists
    if (ins->targetType == Compiler::StaticType::Void)
        return ins->next;

    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(ins->args.size() == 2);
    const auto &indexArg = ins->args[0];
    const auto &valueArg = ins->args[1];
    Compiler::StaticType type = m_utils.optimizeRegisterType(valueArg.second);
    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    Compiler::StaticType listType = ins->targetType;

    // Range check
    llvm::Value *indexDouble = m_utils.castValue(indexArg.second, indexArg.first);
    llvm::Value *indexInt = getIndex(listPtr, indexDouble);
    llvm::Value *inRange = createSizeRangeCheck(listPtr, indexInt, "listReplace.indexInRange");

    llvm::BasicBlock *replaceBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(inRange, replaceBlock, nextBlock);

    // Replace
    m_builder.SetInsertPoint(replaceBlock);

    llvm::Value *itemPtr = m_utils.getListItem(listPtr, indexInt);
    llvm::Value *typePtr = m_utils.getValueTypePtr(itemPtr);
    llvm::Value *loadedType = m_builder.CreateLoad(m_builder.getInt32Ty(), typePtr);
    llvm::Value *typeVar = createListTypeVar(listPtr, loadedType);

    createListTypeAssumption(listPtr, typeVar, ins->targetType);
    m_utils.createValueStore(itemPtr, typeVar, valueArg.second, listType, type);

    // Value store may change type, make sure to update it
    loadedType = m_builder.CreateLoad(m_builder.getInt32Ty(), typeVar);
    m_builder.CreateStore(loadedType, typePtr);

    createListTypeUpdate(listPtr, valueArg.second, type);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);
    return ins->next;
}

LLVMInstruction *Lists::buildGetListContents(LLVMInstruction *ins)
{
    assert(ins->args.size() == 0);
    const LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);
    llvm::Value *ptr = m_builder.CreateCall(m_utils.functions().resolve_list_to_string(), listPtr.ptr);
    m_utils.freeStringLater(ptr);
    ins->functionReturnReg->value = ptr;

    return ins->next;
}

LLVMInstruction *Lists::buildGetListItem(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    // Return empty string for empty lists
    if (ins->targetType == Compiler::StaticType::Void) {
        LLVMConstantRegister nullReg(Compiler::StaticType::String, "");
        ins->functionReturnReg->value = m_utils.createValue(static_cast<LLVMRegister *>(&nullReg));
        return ins->next;
    }

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    // Range check
    llvm::Value *indexDouble = m_utils.castValue(arg.second, arg.first);
    llvm::Value *indexInt = getIndex(listPtr, indexDouble);
    llvm::Value *inRange = createSizeRangeCheck(listPtr, indexInt, "getListItem.indexInRange");

    llvm::BasicBlock *inRangeBlock = llvm::BasicBlock::Create(llvmCtx, "getListItem.inRange", function);
    llvm::BasicBlock *outOfRangeBlock = llvm::BasicBlock::Create(llvmCtx, "getListItem.outOfRange", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "getListItem.next", function);
    m_builder.CreateCondBr(inRange, inRangeBlock, outOfRangeBlock);

    // In range
    m_builder.SetInsertPoint(inRangeBlock);
    llvm::Value *itemPtr = m_utils.getListItem(listPtr, indexInt);
    llvm::Value *itemType = m_builder.CreateLoad(m_builder.getInt32Ty(), m_utils.getValueTypePtr(itemPtr));
    m_builder.CreateBr(nextBlock);

    // Out of range
    m_builder.SetInsertPoint(outOfRangeBlock);
    LLVMConstantRegister emptyStringReg(Compiler::StaticType::String, "");
    llvm::Value *emptyString = m_utils.createValue(static_cast<LLVMRegister *>(&emptyStringReg));
    llvm::Value *stringType = m_builder.getInt32(static_cast<uint32_t>(ValueType::String));
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);

    // Result
    llvm::PHINode *result = m_builder.CreatePHI(itemPtr->getType(), 2, "getListItem.result");
    result->addIncoming(itemPtr, inRangeBlock);
    result->addIncoming(emptyString, outOfRangeBlock);

    llvm::PHINode *itemTypeResult = m_builder.CreatePHI(m_builder.getInt32Ty(), 2, "getListItem.itemType");
    itemTypeResult->addIncoming(itemType, inRangeBlock);
    itemTypeResult->addIncoming(stringType, outOfRangeBlock);

    llvm::Value *typeVar = createListTypeVar(listPtr, itemTypeResult);
    ins->functionReturnReg->value = result;
    ins->functionReturnReg->typeVar = typeVar;
    createListTypeAssumption(listPtr, typeVar, ins->targetType, inRange);

    return ins->next;
}

LLVMInstruction *Lists::buildGetListSize(LLVMInstruction *ins)
{
    assert(ins->args.size() == 0);
    const LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);
    llvm::Value *size = m_utils.getListSize(listPtr);
    ins->functionReturnReg->value = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());

    return ins->next;
}

LLVMInstruction *Lists::buildGetListItemIndex(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    Compiler::StaticType listType = ins->targetType;

    ins->functionReturnReg->value = m_builder.CreateSIToFP(m_utils.getListItemIndex(listPtr, listType, arg.second), m_builder.getDoubleTy());
    return ins->next;
}

LLVMInstruction *Lists::buildListContainsItem(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    Compiler::StaticType listType = ins->targetType;

    llvm::Value *index = m_utils.getListItemIndex(listPtr, listType, arg.second);
    ins->functionReturnReg->value = m_builder.CreateICmpSGT(index, llvm::ConstantInt::get(m_builder.getInt64Ty(), -1, true));

    return ins->next;
}

llvm::Value *Lists::getIndex(const LLVMListPtr &listPtr, llvm::Value *indexDouble)
{
    llvm::Value *zero = llvm::ConstantFP::get(m_utils.llvmCtx(), llvm::APFloat(0.0));
    llvm::Value *isNegative = m_builder.CreateFCmpOLT(indexDouble, zero, "listIndex.isNegative");
    llvm::Value *intMax = llvm::ConstantInt::get(m_builder.getInt64Ty(), INT64_MAX);
    llvm::Value *intIndex = m_builder.CreateFPToUI(indexDouble, m_builder.getInt64Ty(), "listIndex.int");
    return m_builder.CreateSelect(isNegative, intMax, intIndex);
}

llvm::Value *Lists::createSizeRangeCheck(const LLVMListPtr &listPtr, llvm::Value *indexInt, const std::string &name, bool includeSize)
{
    llvm::Value *size = m_utils.getListSize(listPtr);

    if (includeSize)
        return m_builder.CreateICmpULE(indexInt, size, name);
    else
        return m_builder.CreateICmpULT(indexInt, size, name);
}

void Lists::createListTypeUpdate(const LLVMListPtr &listPtr, const LLVMRegister *newValue, Compiler::StaticType newValueType)
{
    if (listPtr.hasNumber && listPtr.hasBool && listPtr.hasString) {
        // Get the new type
        llvm::Value *newType;

        if (newValue->isRawValue)
            newType = m_builder.getInt32(static_cast<uint32_t>(m_utils.mapType(newValueType)));
        else {
            llvm::Value *typeField = m_builder.CreateStructGEP(m_utils.compilerCtx()->valueDataType(), newValue->value, 1);
            newType = m_builder.CreateLoad(m_builder.getInt32Ty(), typeField);
        }

        bool staticHasNumber = (newValueType & Compiler::StaticType::Number) == Compiler::StaticType::Number;
        bool staticHasBool = (newValueType & Compiler::StaticType::Bool) == Compiler::StaticType::Bool;
        bool staticHasString = (newValueType & Compiler::StaticType::String) == Compiler::StaticType::String;

        llvm::Value *isNumber;

        if (staticHasNumber)
            isNumber = m_builder.CreateICmpEQ(newType, m_builder.getInt32(static_cast<uint32_t>(ValueType::Number)));
        else
            isNumber = m_builder.getInt1(false);

        llvm::Value *isBool;

        if (staticHasBool)
            isBool = m_builder.CreateICmpEQ(newType, m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool)));
        else
            isBool = m_builder.getInt1(false);

        llvm::Value *isString;

        if (staticHasString)
            isString = m_builder.CreateICmpEQ(newType, m_builder.getInt32(static_cast<uint32_t>(ValueType::String)));
        else
            isString = m_builder.getInt1(false);

        // Update flags
        llvm::Value *previous = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.hasNumber);
        m_builder.CreateStore(m_builder.CreateOr(previous, isNumber), listPtr.hasNumber);

        previous = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.hasBool);
        m_builder.CreateStore(m_builder.CreateOr(previous, isBool), listPtr.hasBool);

        previous = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.hasString);
        m_builder.CreateStore(m_builder.CreateOr(previous, isString), listPtr.hasString);
    }
}

llvm::Value *Lists::createListTypeVar(const LLVMListPtr &listPtr, llvm::Value *type)
{
    llvm::Value *typeVar = m_utils.addAlloca(m_builder.getInt32Ty());
    m_builder.CreateStore(type, typeVar);
    return typeVar;
}

void Lists::createListTypeAssumption(const LLVMListPtr &listPtr, llvm::Value *typeVar, Compiler::StaticType staticType, llvm::Value *inRange)
{
    if (listPtr.hasNumber && listPtr.hasBool && listPtr.hasString) {
        llvm::Function *assumeIntrinsic = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::assume);

        // Load the compile time list type information
        bool staticHasNumber = (staticType & Compiler::StaticType::Number) == Compiler::StaticType::Number;
        bool staticHasBool = (staticType & Compiler::StaticType::Bool) == Compiler::StaticType::Bool;
        bool staticHasString = (staticType & Compiler::StaticType::String) == Compiler::StaticType::String;

        // Load the runtime list type information
        llvm::Value *hasNumber;

        if (staticHasNumber)
            hasNumber = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.hasNumber);
        else
            hasNumber = m_builder.getInt1(false);

        llvm::Value *hasBool;

        if (staticHasBool)
            hasBool = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.hasBool);
        else
            hasBool = m_builder.getInt1(false);

        llvm::Value *hasString;

        if (staticHasString)
            hasString = m_builder.CreateLoad(m_builder.getInt1Ty(), listPtr.hasString);
        else
            hasString = m_builder.getInt1(false);

        llvm::Value *type = m_builder.CreateLoad(m_builder.getInt32Ty(), typeVar);

        if (!inRange)
            inRange = m_builder.getInt1(true);

        llvm::Value *numberType = m_builder.getInt32(static_cast<uint32_t>(ValueType::Number));
        llvm::Value *boolType = m_builder.getInt32(static_cast<uint32_t>(ValueType::Bool));
        llvm::Value *stringType = m_builder.getInt32(static_cast<uint32_t>(ValueType::String));

        // Create assumptions
        llvm::BasicBlock *outOfRangeBlock = llvm::BasicBlock::Create(m_utils.llvmCtx(), "outOfRange", m_utils.function());
        llvm::BasicBlock *inRangeBlock = llvm::BasicBlock::Create(m_utils.llvmCtx(), "inRange", m_utils.function());
        llvm::BasicBlock *afterBlock = llvm::BasicBlock::Create(m_utils.llvmCtx(), "afterAssume", m_utils.function());

        m_builder.CreateCondBr(inRange, inRangeBlock, outOfRangeBlock);

        // In-range assumptions
        m_builder.SetInsertPoint(inRangeBlock);

        auto assume = [&](llvm::Value *cond) { m_builder.CreateCall(assumeIntrinsic, cond); };

        llvm::Value *notNumber = m_builder.CreateNot(hasNumber);
        llvm::Value *notBool = m_builder.CreateNot(hasBool);
        llvm::Value *notString = m_builder.CreateNot(hasString);

        // if (!hasBool && !hasString) type == Number
        llvm::Value *cond1 = m_builder.CreateAnd(notBool, notString);
        llvm::Value *assume1 = m_builder.CreateICmpEQ(type, numberType);
        assume(m_builder.CreateSelect(cond1, assume1, m_builder.getInt1(true)));

        // else if (!hasNumber && !hasString) type == Bool
        llvm::Value *cond2 = m_builder.CreateAnd(notNumber, notString);
        llvm::Value *assume2 = m_builder.CreateICmpEQ(type, boolType);
        assume(m_builder.CreateSelect(cond2, assume2, m_builder.getInt1(true)));

        // else if (!hasNumber && !hasBool) type == String
        llvm::Value *cond3 = m_builder.CreateAnd(notNumber, notBool);
        llvm::Value *assume3 = m_builder.CreateICmpEQ(type, stringType);
        assume(m_builder.CreateSelect(cond3, assume3, m_builder.getInt1(true)));

        // else if (!hasBool) type == Number || type == String
        llvm::Value *cond4 = notBool;
        llvm::Value *assume4 = m_builder.CreateOr(m_builder.CreateICmpEQ(type, numberType), m_builder.CreateICmpEQ(type, stringType));
        assume(m_builder.CreateSelect(cond4, assume4, m_builder.getInt1(true)));

        // else if (!hasNumber) type == Bool || type == String
        llvm::Value *cond5 = notNumber;
        llvm::Value *assume5 = m_builder.CreateOr(m_builder.CreateICmpEQ(type, boolType), m_builder.CreateICmpEQ(type, stringType));
        assume(m_builder.CreateSelect(cond5, assume5, m_builder.getInt1(true)));

        // else if (!hasString) type == Number || type == Bool
        llvm::Value *cond6 = notString;
        llvm::Value *assume6 = m_builder.CreateOr(m_builder.CreateICmpEQ(type, numberType), m_builder.CreateICmpEQ(type, boolType));
        assume(m_builder.CreateSelect(cond6, assume6, m_builder.getInt1(true)));

        m_builder.CreateBr(afterBlock);

        // Out-of-range: always string
        m_builder.SetInsertPoint(outOfRangeBlock);
        llvm::Value *isString = m_builder.CreateICmpEQ(type, stringType);
        assume(isString);
        m_builder.CreateBr(afterBlock);

        m_builder.SetInsertPoint(afterBlock);
    }
}
