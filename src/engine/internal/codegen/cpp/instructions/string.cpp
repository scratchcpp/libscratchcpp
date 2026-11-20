// SPDX-License-Identifier: Apache-2.0

#include "string.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult String::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::StringConcat:
            ret.next = buildStringConcat(ins);
            break;

        case CppInstruction::Type::StringChar:
            ret.next = buildStringChar(ins);
            break;

        case CppInstruction::Type::StringLength:
            ret.next = buildStringLength(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *String::buildStringConcat(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *String::buildStringChar(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *String::buildStringLength(CppInstruction *ins)
{
    return ins->getNext();
}
