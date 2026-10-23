// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class String : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildStringConcat(LLVMInstruction *ins);
        LLVMInstruction *buildStringChar(LLVMInstruction *ins);
        LLVMInstruction *buildStringLength(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
