// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class Math : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildAdd(CppInstruction *ins);
        CppInstruction *buildSub(CppInstruction *ins);
        CppInstruction *buildMul(CppInstruction *ins);
        CppInstruction *buildDiv(CppInstruction *ins);
        CppInstruction *buildRandom(CppInstruction *ins);
        CppInstruction *buildRandomInt(CppInstruction *ins);
        CppInstruction *buildMod(CppInstruction *ins);
        CppInstruction *buildRound(CppInstruction *ins);
        CppInstruction *buildAbs(CppInstruction *ins);
        CppInstruction *buildFloor(CppInstruction *ins);
        CppInstruction *buildCeil(CppInstruction *ins);
        CppInstruction *buildSqrt(CppInstruction *ins);
        CppInstruction *buildSin(CppInstruction *ins);
        CppInstruction *buildCos(CppInstruction *ins);
        CppInstruction *buildTan(CppInstruction *ins);
        CppInstruction *buildAsin(CppInstruction *ins);
        CppInstruction *buildAcos(CppInstruction *ins);
        CppInstruction *buildAtan(CppInstruction *ins);
        CppInstruction *buildLn(CppInstruction *ins);
        CppInstruction *buildLog10(CppInstruction *ins);
        CppInstruction *buildExp(CppInstruction *ins);
        CppInstruction *buildExp10(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
