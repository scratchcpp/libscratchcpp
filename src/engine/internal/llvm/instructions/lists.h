// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
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
};

} // namespace libscratchcpp::llvmins
