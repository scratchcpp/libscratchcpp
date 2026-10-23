// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

#include "instructiongroup.h"

namespace libscratchcpp
{

class LLVMListPtr;
class LLVMRegister;

namespace llvmins
{

class Lists : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildClearList(LLVMInstruction *ins);
        LLVMInstruction *buildRemoveListItem(LLVMInstruction *ins);
        LLVMInstruction *buildAppendToList(LLVMInstruction *ins);
        LLVMInstruction *buildInsertToList(LLVMInstruction *ins);
        LLVMInstruction *buildListReplace(LLVMInstruction *ins);
        LLVMInstruction *buildGetListContents(LLVMInstruction *ins);
        LLVMInstruction *buildGetListItem(LLVMInstruction *ins);
        LLVMInstruction *buildGetListSize(LLVMInstruction *ins);
        LLVMInstruction *buildGetListItemIndex(LLVMInstruction *ins);
        LLVMInstruction *buildListContainsItem(LLVMInstruction *ins);

        llvm::Value *createIndexRangeCheck(const LLVMListPtr &listPtr, llvm::Value *index, const std::string &name, bool includeSize = false);

        void createListTypeUpdate(const LLVMListPtr &listPtr, const LLVMRegister *newValue, Compiler::StaticType newValueType);
        llvm::Value *createListTypeVar(const LLVMListPtr &listPtr, llvm::Value *type);
        void createListTypeAssumption(const LLVMListPtr &listPtr, llvm::Value *typeVar, Compiler::StaticType staticType, llvm::Value *inRange = nullptr);
};

} // namespace llvmins

} // namespace libscratchcpp
