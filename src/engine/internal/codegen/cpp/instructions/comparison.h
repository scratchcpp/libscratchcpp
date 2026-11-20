// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class Comparison : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildCmpEQ(CppInstruction *ins);
        CppInstruction *buildCmpGT(CppInstruction *ins);
        CppInstruction *buildCmpLT(CppInstruction *ins);
        CppInstruction *buildStrCmpEQCS(CppInstruction *ins);
        CppInstruction *buildStrCmpEQCI(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
