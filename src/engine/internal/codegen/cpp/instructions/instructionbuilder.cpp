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
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

InstructionBuilder::InstructionBuilder()
{
    // Create groups
    m_groups.push_back(std::make_shared<Functions>());
    m_groups.push_back(std::make_shared<Math>());
    m_groups.push_back(std::make_shared<Comparison>());
    m_groups.push_back(std::make_shared<String>());
    m_groups.push_back(std::make_shared<Logic>());
    m_groups.push_back(std::make_shared<Control>());
    m_groups.push_back(std::make_shared<Variables>());
    m_groups.push_back(std::make_shared<Lists>());
    m_groups.push_back(std::make_shared<Procedures>());
}

CppInstruction *InstructionBuilder::process(CppInstruction *ins)
{
    // Process all groups
    for (const auto &group : m_groups) {
        ProcessResult result = group->process(ins);

        if (result.match) {
            // TODO: Add integer support
            return result.next;
        }
    }

    assert(false); // instruction not found
    return ins;
}
