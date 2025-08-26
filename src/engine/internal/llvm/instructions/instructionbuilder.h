// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "instructiongroup.h"

namespace libscratchcpp::llvmins
{

class InstructionBuilder
{
    public:
        InstructionBuilder(LLVMBuildUtils &utils);

        LLVMInstruction *process(LLVMInstruction *ins);

    private:
        LLVMBuildUtils &m_utils;
        std::vector<std::shared_ptr<InstructionGroup>> m_groups;
};

} // namespace libscratchcpp::llvmins
