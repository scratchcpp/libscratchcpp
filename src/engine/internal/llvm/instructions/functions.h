// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class Functions : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildFunctionCall(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
