// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class Control : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildSelect(LLVMInstruction *ins);
        LLVMInstruction *buildYield(LLVMInstruction *ins);
        LLVMInstruction *buildBeginIf(LLVMInstruction *ins);
        LLVMInstruction *buildBeginElse(LLVMInstruction *ins);
        LLVMInstruction *buildEndIf(LLVMInstruction *ins);
        LLVMInstruction *buildBeginRepeatLoop(LLVMInstruction *ins);
        LLVMInstruction *buildLoopIndex(LLVMInstruction *ins);
        LLVMInstruction *buildBeginWhileLoop(LLVMInstruction *ins);
        LLVMInstruction *buildBeginRepeatUntilLoop(LLVMInstruction *ins);
        LLVMInstruction *buildBeginLoopCondition(LLVMInstruction *ins);
        LLVMInstruction *buildEndLoop(LLVMInstruction *ins);
        LLVMInstruction *buildStop(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
