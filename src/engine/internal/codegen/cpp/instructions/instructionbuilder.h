// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

#include "instructiongroup.h"

namespace libscratchcpp::cppins
{

class InstructionBuilder
{
    public:
        InstructionBuilder(CppCodeGenerator &codeGen);

        CppInstruction *process(CppInstruction *ins);

    private:
        std::vector<std::shared_ptr<InstructionGroup>> m_groups;
};

} // namespace libscratchcpp::cppins
