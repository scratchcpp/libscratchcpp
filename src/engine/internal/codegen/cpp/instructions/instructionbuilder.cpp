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

InstructionBuilder::InstructionBuilder(CppCodeGenerator &codeGen)
{
    // Create groups
    m_groups.push_back(std::make_shared<Functions>(codeGen));
    m_groups.push_back(std::make_shared<Math>(codeGen));
    m_groups.push_back(std::make_shared<Comparison>(codeGen));
    m_groups.push_back(std::make_shared<String>(codeGen));
    m_groups.push_back(std::make_shared<Logic>(codeGen));
    m_groups.push_back(std::make_shared<Control>(codeGen));
    m_groups.push_back(std::make_shared<Variables>(codeGen));
    m_groups.push_back(std::make_shared<Lists>(codeGen));
    m_groups.push_back(std::make_shared<Procedures>(codeGen));
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
