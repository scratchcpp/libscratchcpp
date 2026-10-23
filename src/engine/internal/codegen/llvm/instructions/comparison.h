// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class Comparison : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildCmpEQ(LLVMInstruction *ins);
        LLVMInstruction *buildCmpGT(LLVMInstruction *ins);
        LLVMInstruction *buildCmpLT(LLVMInstruction *ins);
        LLVMInstruction *buildStrCmpEQCS(LLVMInstruction *ins);
        LLVMInstruction *buildStrCmpEQCI(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
