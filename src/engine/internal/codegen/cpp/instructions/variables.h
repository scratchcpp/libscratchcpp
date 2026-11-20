// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class Variables : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildCreateLocalVariable(CppInstruction *ins);
        CppInstruction *buildWriteLocalVariable(CppInstruction *ins);
        CppInstruction *buildReadLocalVariable(CppInstruction *ins);
        CppInstruction *buildWriteVariable(CppInstruction *ins);
        CppInstruction *buildReadVariable(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
