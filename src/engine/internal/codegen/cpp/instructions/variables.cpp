// SPDX-License-Identifier: Apache-2.0

#include "variables.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Variables::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::CreateLocalVariable:
            ret.next = buildCreateLocalVariable(ins);
            break;

        case CppInstruction::Type::WriteLocalVariable:
            ret.next = buildWriteLocalVariable(ins);
            break;

        case CppInstruction::Type::ReadLocalVariable:
            ret.next = buildReadLocalVariable(ins);
            break;

        case CppInstruction::Type::WriteVariable:
            ret.next = buildWriteVariable(ins);
            break;

        case CppInstruction::Type::ReadVariable:
            ret.next = buildReadVariable(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Variables::buildCreateLocalVariable(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Variables::buildWriteLocalVariable(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Variables::buildReadLocalVariable(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Variables::buildWriteVariable(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Variables::buildReadVariable(CppInstruction *ins)
{
    return ins->getNext();
}
