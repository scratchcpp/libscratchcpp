// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class Logic : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildAnd(CppInstruction *ins);
        CppInstruction *buildOr(CppInstruction *ins);
        CppInstruction *buildNot(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
