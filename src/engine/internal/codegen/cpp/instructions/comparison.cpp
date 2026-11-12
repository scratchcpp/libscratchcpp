// SPDX-License-Identifier: Apache-2.0

#include "comparison.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Comparison::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::CmpEQ:
            ret.next = buildCmpEQ(ins);
            break;

        case CppInstruction::Type::CmpGT:
            ret.next = buildCmpGT(ins);
            break;

        case CppInstruction::Type::CmpLT:
            ret.next = buildCmpLT(ins);
            break;

        case CppInstruction::Type::StrCmpEQCS:
            ret.next = buildStrCmpEQCS(ins);
            break;

        case CppInstruction::Type::StrCmpEQCI:
            ret.next = buildStrCmpEQCI(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Comparison::buildCmpEQ(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Comparison::buildCmpGT(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Comparison::buildCmpLT(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Comparison::buildStrCmpEQCS(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Comparison::buildStrCmpEQCI(CppInstruction *ins)
{
    return ins->getNext();
}
