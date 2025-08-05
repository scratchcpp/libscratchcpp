// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class Variables : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildCreateLocalVariable(LLVMInstruction *ins);
        LLVMInstruction *buildWriteLocalVariable(LLVMInstruction *ins);
        LLVMInstruction *buildReadLocalVariable(LLVMInstruction *ins);
        LLVMInstruction *buildWriteVariable(LLVMInstruction *ins);
        LLVMInstruction *buildReadVariable(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
