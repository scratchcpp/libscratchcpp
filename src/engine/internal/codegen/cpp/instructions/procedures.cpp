// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/blockprototype.h>

#include "procedures.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Procedures::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::CallProcedure:
            ret.next = buildCallProcedure(ins);
            break;

        case CppInstruction::Type::ProcedureArg:
            ret.next = buildProcedureArg(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Procedures::buildCallProcedure(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Procedures::buildProcedureArg(CppInstruction *ins)
{
    return ins->getNext();
}
