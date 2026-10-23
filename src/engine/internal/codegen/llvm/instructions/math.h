// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class Math : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(LLVMInstruction *ins) override;

    private:
        LLVMInstruction *buildAdd(LLVMInstruction *ins);
        LLVMInstruction *buildSub(LLVMInstruction *ins);
        LLVMInstruction *buildMul(LLVMInstruction *ins);
        LLVMInstruction *buildDiv(LLVMInstruction *ins);
        LLVMInstruction *buildRandom(LLVMInstruction *ins);
        LLVMInstruction *buildRandomInt(LLVMInstruction *ins);
        LLVMInstruction *buildMod(LLVMInstruction *ins);
        LLVMInstruction *buildRound(LLVMInstruction *ins);
        LLVMInstruction *buildAbs(LLVMInstruction *ins);
        LLVMInstruction *buildFloor(LLVMInstruction *ins);
        LLVMInstruction *buildCeil(LLVMInstruction *ins);
        LLVMInstruction *buildSqrt(LLVMInstruction *ins);
        LLVMInstruction *buildSin(LLVMInstruction *ins);
        LLVMInstruction *buildCos(LLVMInstruction *ins);
        LLVMInstruction *buildTan(LLVMInstruction *ins);
        LLVMInstruction *buildAsin(LLVMInstruction *ins);
        LLVMInstruction *buildAcos(LLVMInstruction *ins);
        LLVMInstruction *buildAtan(LLVMInstruction *ins);
        LLVMInstruction *buildLn(LLVMInstruction *ins);
        LLVMInstruction *buildLog10(LLVMInstruction *ins);
        LLVMInstruction *buildExp(LLVMInstruction *ins);
        LLVMInstruction *buildExp10(LLVMInstruction *ins);
};

} // namespace libscratchcpp::llvmins
