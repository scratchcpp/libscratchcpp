// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class Functions : public InstructionGroup
{
    public:
        using InstructionGroup::InstructionGroup;

        ProcessResult process(CppInstruction *ins) override;

    private:
        CppInstruction *buildFunctionCall(CppInstruction *ins);
};

} // namespace libscratchcpp::cppins
