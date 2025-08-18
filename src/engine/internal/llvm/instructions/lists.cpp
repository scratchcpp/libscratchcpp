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

        if (listPtr.type) {
            // Update type
            m_builder.CreateStore(m_builder.getInt32(static_cast<uint32_t>(ValueType::Void)), listPtr.type);
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
    llvm::Value *min = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Value *size = m_utils.getListSize(listPtr);
    size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
    llvm::Value *index = m_utils.castValue(arg.second, arg.first);
    llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLT(index, size));
    llvm::BasicBlock *removeBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(inRange, removeBlock, nextBlock);

    // Remove
    m_builder.SetInsertPoint(removeBlock);
    index = m_builder.CreateFPToUI(m_utils.castValue(arg.second, arg.first), m_builder.getInt64Ty());
    m_builder.CreateCall(m_utils.functions().resolve_list_remove(), { listPtr.ptr, index });

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
    // NOTE: Items created using appendEmpty() are always numbers
    m_utils.createValueStore(itemPtr, m_utils.getValueTypePtr(itemPtr), arg.second, Compiler::StaticType::Number, type);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);

    if (listPtr.size) {
        // Update local size
        llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.size);
        size = m_builder.CreateAdd(size, m_builder.getInt64(1));
        m_builder.CreateStore(size, listPtr.size);
    }

    createListTypeUpdate(listPtr, arg.second);
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
    llvm::Value *size = m_utils.getListSize(listPtr);
    llvm::Value *min = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
    llvm::Value *index = m_utils.castValue(indexArg.second, indexArg.first);
    llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLE(index, size));
    llvm::BasicBlock *insertBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(inRange, insertBlock, nextBlock);

    // Insert
    m_builder.SetInsertPoint(insertBlock);
    index = m_builder.CreateFPToUI(index, m_builder.getInt64Ty());
    llvm::Value *itemPtr = m_builder.CreateCall(m_utils.functions().resolve_list_insert_empty(), { listPtr.ptr, index });
    m_utils.createValueStore(itemPtr, m_utils.getValueTypePtr(itemPtr), valueArg.second, type);

    if (listPtr.size) {
        // Update size
        llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.size);
        size = m_builder.CreateAdd(size, m_builder.getInt64(1));
        m_builder.CreateStore(size, listPtr.size);
    }

    createListTypeUpdate(listPtr, valueArg.second);
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
    llvm::Value *min = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Value *size = m_utils.getListSize(listPtr);
    size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
    llvm::Value *index = m_utils.castValue(indexArg.second, indexArg.first);
    llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLT(index, size));
    llvm::BasicBlock *replaceBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(inRange, replaceBlock, nextBlock);

    // Replace
    m_builder.SetInsertPoint(replaceBlock);
    index = m_builder.CreateFPToUI(index, m_builder.getInt64Ty());

    llvm::Value *itemPtr = m_utils.getListItem(listPtr, index);
    llvm::Value *itemType = m_builder.CreateLoad(m_builder.getInt32Ty(), m_utils.getValueTypePtr(itemPtr));
    llvm::Value *typeVar = createListTypeVar(listPtr, itemType);
    createListTypeAssumption(listPtr, itemType, typeVar);

    m_utils.createValueStore(itemPtr, m_utils.getValueTypePtr(itemPtr), valueArg.second, listType, type);
    createListTypeUpdate(listPtr, valueArg.second);
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
    // Return empty string for empty lists
    if (ins->targetType == Compiler::StaticType::Void) {
        LLVMConstantRegister nullReg(Compiler::StaticType::String, "");
        ins->functionReturnReg->value = m_utils.createValue(static_cast<LLVMRegister *>(&nullReg));
        return ins->next;
    }

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->targetList);

    llvm::Value *min = llvm::ConstantFP::get(m_utils.llvmCtx(), llvm::APFloat(0.0));
    llvm::Value *size = m_utils.getListSize(listPtr);
    size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
    llvm::Value *index = m_utils.castValue(arg.second, arg.first);
    llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLT(index, size), "inRange");

    LLVMConstantRegister nullReg(Compiler::StaticType::String, "");
    llvm::Value *null = m_utils.createValue(static_cast<LLVMRegister *>(&nullReg));

    index = m_builder.CreateFPToUI(index, m_builder.getInt64Ty());

    llvm::Value *itemPtr = m_builder.CreateSelect(inRange, m_utils.getListItem(listPtr, index), null);
    llvm::Value *stringType = m_builder.getInt32(static_cast<uint32_t>(ValueType::String));
    llvm::Value *type = m_builder.CreateSelect(inRange, m_builder.CreateLoad(m_builder.getInt32Ty(), m_utils.getValueTypePtr(itemPtr)), stringType);
    llvm::Value *typeVar = createListTypeVar(listPtr, type);

    ins->functionReturnReg->value = itemPtr;
    ins->functionReturnReg->typeVar = typeVar;
    createListTypeAssumption(listPtr, type, typeVar, inRange);

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

void Lists::createListTypeUpdate(const LLVMListPtr &listPtr, const LLVMRegister *newValue)
{
    if (listPtr.type) {
        // Update type
        llvm::Value *currentType = m_builder.CreateLoad(m_builder.getInt32Ty(), listPtr.type);
        llvm::Value *newTypeFlag;

        if (newValue->isRawValue)
            newTypeFlag = m_builder.getInt32(static_cast<uint32_t>(m_utils.mapType(newValue->type())));
        else {
            llvm::Value *typeField = m_builder.CreateStructGEP(m_utils.compilerCtx()->valueDataType(), newValue->value, 1);
            newTypeFlag = m_builder.CreateLoad(m_builder.getInt32Ty(), typeField);
        }

        m_builder.CreateStore(m_builder.CreateOr(currentType, newTypeFlag), listPtr.type);
    }
}

llvm::Value *Lists::createListTypeVar(const LLVMListPtr &listPtr, llvm::Value *itemType)
{
    llvm::Value *typeVar = m_utils.addAlloca(m_builder.getInt32Ty());
    m_builder.CreateStore(itemType, typeVar);
    return typeVar;
}

void Lists::createListTypeAssumption(const LLVMListPtr &listPtr, llvm::Value *itemType, llvm::Value *typeVar, llvm::Value *inRange)
{
    if (listPtr.type) {
        llvm::Function *assumeIntrinsic = llvm::Intrinsic::getDeclaration(m_utils.module(), llvm::Intrinsic::assume);

        // Load the runtime list type information
        llvm::Value *listTypeFlags = m_builder.CreateLoad(m_builder.getInt32Ty(), listPtr.type);

        // Create assumption that the item type is contained in the list type flags
        llvm::Value *typeIsValid = m_builder.CreateICmpEQ(m_builder.CreateAnd(listTypeFlags, itemType), itemType);

        if (inRange) {
            llvm::Value *stringType = m_builder.getInt32(static_cast<uint32_t>(ValueType::String));
            llvm::Value *canNotBeString = m_builder.CreateICmpNE(m_builder.CreateAnd(listTypeFlags, stringType), stringType);
            llvm::Value *isString = m_builder.CreateICmpEQ(itemType, stringType);
            llvm::Value *impossible = m_builder.CreateAnd(m_builder.CreateAnd(inRange, canNotBeString), isString);
            typeIsValid = m_builder.CreateAnd(typeIsValid, m_builder.CreateNot(impossible));
        }

        m_builder.CreateCall(assumeIntrinsic, typeIsValid);
    }
}
