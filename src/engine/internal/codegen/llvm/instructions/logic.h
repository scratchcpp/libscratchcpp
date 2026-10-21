// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class Logic : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildAnd(LLVMInstruction *ins);
        LLVMInstruction *buildOr(LLVMInstruction *ins);
        LLVMInstruction *buildNot(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
