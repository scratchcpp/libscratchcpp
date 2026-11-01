// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "processresult.h"

namespace libscratchcpp::cppins
{

class InstructionGroup
{
    public:
        InstructionGroup();

        virtual ProcessResult process(CppInstruction *ins) = 0;
};

} // namespace libscratchcpp::cppins
