// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class String : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildStringConcat(CppInstruction *ins);
        CppInstruction *buildStringChar(CppInstruction *ins);
        CppInstruction *buildStringLength(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
