// SPDX-License-Identifier: Apache-2.0

#include "lists.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"
#include "../llvmconstantregister.h"

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
    assert(ins->args.size() == 0);
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);
    m_builder.CreateCall(m_utils.functions().resolve_list_clear(), listPtr.ptr);

    return ins->next;
}

LLVMInstruction *Lists::buildRemoveListItem(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

    // Range check
    llvm::Value *min = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
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
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

    Compiler::StaticType listType = ins->targetType;

    // Check if enough space is allocated
    llvm::Value *allocatedSize = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.allocatedSizePtr);
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
    llvm::Value *isAllocated = m_builder.CreateICmpUGT(allocatedSize, size);
    llvm::BasicBlock *ifBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(llvmCtx, "", function);
    m_builder.CreateCondBr(isAllocated, ifBlock, elseBlock);

    // If there's enough space, use the allocated memory
    m_builder.SetInsertPoint(ifBlock);
    llvm::Value *itemPtr = m_utils.getListItem(listPtr, size);
    m_utils.createReusedValueStore(arg.second, itemPtr, type, listType);
    m_builder.CreateStore(m_builder.CreateAdd(size, m_builder.getInt64(1)), listPtr.sizePtr);
    m_builder.CreateBr(nextBlock);

    // Otherwise call appendEmpty()
    m_builder.SetInsertPoint(elseBlock);
    itemPtr = m_builder.CreateCall(m_utils.functions().resolve_list_append_empty(), listPtr.ptr);
    m_utils.createReusedValueStore(arg.second, itemPtr, type, listType);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);
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
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

    Compiler::StaticType listType = ins->targetType;

    // Range check
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
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
    m_utils.createReusedValueStore(valueArg.second, itemPtr, type, listType);

    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);
    return ins->next;
}

LLVMInstruction *Lists::buildListReplace(LLVMInstruction *ins)
{
    llvm::LLVMContext &llvmCtx = m_utils.llvmCtx();
    llvm::Function *function = m_utils.function();

    assert(ins->args.size() == 2);
    const auto &indexArg = ins->args[0];
    const auto &valueArg = ins->args[1];
    Compiler::StaticType type = m_utils.optimizeRegisterType(valueArg.second);
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

    Compiler::StaticType listType = ins->targetType;

    // Range check
    llvm::Value *min = llvm::ConstantFP::get(llvmCtx, llvm::APFloat(0.0));
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
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
    m_utils.createValueStore(valueArg.second, itemPtr, type, listType);
    m_builder.CreateBr(nextBlock);

    m_builder.SetInsertPoint(nextBlock);
    return ins->next;
}

LLVMInstruction *Lists::buildGetListContents(LLVMInstruction *ins)
{
    assert(ins->args.size() == 0);
    const LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);
    llvm::Value *ptr = m_builder.CreateCall(m_utils.functions().resolve_list_to_string(), listPtr.ptr);
    m_utils.freeStringLater(ptr);
    ins->functionReturnReg->value = ptr;

    return ins->next;
}

LLVMInstruction *Lists::buildGetListItem(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

    Compiler::StaticType listType = ins->functionReturnReg->type();

    llvm::Value *min = llvm::ConstantFP::get(m_utils.llvmCtx(), llvm::APFloat(0.0));
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
    size = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());
    llvm::Value *index = m_utils.castValue(arg.second, arg.first);
    llvm::Value *inRange = m_builder.CreateAnd(m_builder.CreateFCmpOGE(index, min), m_builder.CreateFCmpOLT(index, size));

    LLVMConstantRegister nullReg(listType == Compiler::StaticType::Unknown ? Compiler::StaticType::Number : listType, Value());
    llvm::Value *null = m_utils.createValue(static_cast<LLVMRegister *>(&nullReg));

    index = m_builder.CreateFPToUI(index, m_builder.getInt64Ty());
    ins->functionReturnReg->value = m_builder.CreateSelect(inRange, m_utils.getListItem(listPtr, index), null);

    return ins->next;
}

LLVMInstruction *Lists::buildGetListSize(LLVMInstruction *ins)
{
    assert(ins->args.size() == 0);
    const LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);
    llvm::Value *size = m_builder.CreateLoad(m_builder.getInt64Ty(), listPtr.sizePtr);
    ins->functionReturnReg->value = m_builder.CreateUIToFP(size, m_builder.getDoubleTy());

    return ins->next;
}

LLVMInstruction *Lists::buildGetListItemIndex(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

    Compiler::StaticType listType = ins->targetType;

    ins->functionReturnReg->value = m_builder.CreateSIToFP(m_utils.getListItemIndex(listPtr, listType, arg.second), m_builder.getDoubleTy());
    return ins->next;
}

LLVMInstruction *Lists::buildListContainsItem(LLVMInstruction *ins)
{
    assert(ins->args.size() == 1);
    const auto &arg = ins->args[0];
    LLVMListPtr &listPtr = m_utils.listPtr(ins->workList);

    Compiler::StaticType listType = ins->targetType;

    llvm::Value *index = m_utils.getListItemIndex(listPtr, listType, arg.second);
    ins->functionReturnReg->value = m_builder.CreateICmpSGT(index, llvm::ConstantInt::get(m_builder.getInt64Ty(), -1, true));

    return ins->next;
}
