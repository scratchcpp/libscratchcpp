// SPDX-License-Identifier: Apache-2.0

#include "instructionbuilder.h"
#include "functions.h"
#include "math.h"
#include "comparison.h"
#include "string.h"
#include "logic.h"
#include "control.h"
#include "variables.h"
#include "lists.h"
#include "procedures.h"
#include "../llvminstruction.h"
#include "../llvmbuildutils.h"

using namespace libscratchcpp;
using namespace libscratchcpp::llvmins;

InstructionBuilder::InstructionBuilder(LLVMBuildUtils &utils) :
    m_utils(utils)
{
    // Create groups
    m_groups.push_back(std::make_shared<Functions>(utils));
    m_groups.push_back(std::make_shared<Math>(utils));
    m_groups.push_back(std::make_shared<Comparison>(utils));
    m_groups.push_back(std::make_shared<String>(utils));
    m_groups.push_back(std::make_shared<Logic>(utils));
    m_groups.push_back(std::make_shared<Control>(utils));
    m_groups.push_back(std::make_shared<Variables>(utils));
    m_groups.push_back(std::make_shared<Lists>(utils));
    m_groups.push_back(std::make_shared<Procedures>(utils));
}

LLVMInstruction *InstructionBuilder::process(LLVMInstruction *ins)
{
    // Process all groups
    for (const auto &group : m_groups) {
        ProcessResult result = group->process(ins);

        if (result.match) {
#ifndef LLVM_INTEGER_SUPPORT
            if (ins->functionReturnReg)
                ins->functionReturnReg->isInt = m_utils.builder().getInt1(false);
#endif

            return result.next;
        }
    }

    assert(false); // instruction not found
    return ins;
}
