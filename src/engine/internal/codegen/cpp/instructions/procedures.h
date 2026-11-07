// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class Procedures : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildCallProcedure(CppInstruction *ins);
        CppInstruction *buildProcedureArg(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
