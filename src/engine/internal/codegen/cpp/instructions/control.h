// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class Control : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildSelect(CppInstruction *ins);
        CppInstruction *buildYield(CppInstruction *ins);
        CppInstruction *buildBeginIf(CppInstruction *ins);
        CppInstruction *buildBeginElse(CppInstruction *ins);
        CppInstruction *buildEndIf(CppInstruction *ins);
        CppInstruction *buildBeginRepeatLoop(CppInstruction *ins);
        CppInstruction *buildLoopIndex(CppInstruction *ins);
        CppInstruction *buildBeginWhileLoop(CppInstruction *ins);
        CppInstruction *buildBeginRepeatUntilLoop(CppInstruction *ins);
        CppInstruction *buildBeginLoopCondition(CppInstruction *ins);
        CppInstruction *buildEndLoop(CppInstruction *ins);
        CppInstruction *buildStop(CppInstruction *ins);
        CppInstruction *buildThreadStop(CppInstruction *ins);
        CppInstruction *buildInvalidateTarget(CppInstruction *ins);
        CppInstruction *buildPrint(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
