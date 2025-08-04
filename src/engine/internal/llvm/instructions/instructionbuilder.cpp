// SPDX-License-Identifier: Apache-2.0

#include "instructionbuilder.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

InstructionBuilder::InstructionBuilder(LLVMBuildUtils &utils)
{
    // Create groups
}

LLVMInstruction *InstructionBuilder::process(LLVMInstruction *ins)
{
    // Process all groups
    for (const auto &group : m_groups) {
        ProcessResult result = group->process(ins);

        if (result.match)
            return result.next;
    }

    assert(false); // instruction not found
    return ins;
}
