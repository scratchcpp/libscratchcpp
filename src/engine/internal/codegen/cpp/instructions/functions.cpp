// SPDX-License-Identifier: Apache-2.0

#include "functions.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Functions::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::FunctionCall:
            ret.next = buildFunctionCall(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Functions::buildFunctionCall(CppInstruction *ins)
{
    return ins->getNext();
}
