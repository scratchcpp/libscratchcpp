// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class Procedures : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildCallProcedure(LLVMInstruction *ins);
        LLVMInstruction *buildProcedureArg(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
