// SPDX-License-Identifier: Apache-2.0

#include "logic.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Logic::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::And:
            ret.next = buildAnd(ins);
            break;

        case CppInstruction::Type::Or:
            ret.next = buildOr(ins);
            break;

        case CppInstruction::Type::Not:
            ret.next = buildNot(ins);
            break;

        default:
            ret.match = false;
            ret.next = (ins);
            break;
    }

    return ret;
}

CppInstruction *Logic::buildAnd(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Logic::buildOr(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Logic::buildNot(CppInstruction *ins)
{
    return ins->getNext();
}
