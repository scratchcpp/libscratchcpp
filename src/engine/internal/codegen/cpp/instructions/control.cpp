// SPDX-License-Identifier: Apache-2.0

#include "control.h"
#include "../cppinstruction.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Control::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::Select:
            ret.next = buildSelect(ins);
            break;

        case CppInstruction::Type::Yield:
            ret.next = buildYield(ins);
            break;

        case CppInstruction::Type::BeginIf:
            ret.next = buildBeginIf(ins);
            break;

        case CppInstruction::Type::BeginElse:
            ret.next = buildBeginElse(ins);
            break;

        case CppInstruction::Type::EndIf:
            ret.next = buildEndIf(ins);
            break;

        case CppInstruction::Type::BeginRepeatLoop:
            ret.next = buildBeginRepeatLoop(ins);
            break;

        case CppInstruction::Type::LoopIndex:
            ret.next = buildLoopIndex(ins);
            break;

        case CppInstruction::Type::BeginWhileLoop:
            ret.next = buildBeginWhileLoop(ins);
            break;

        case CppInstruction::Type::BeginRepeatUntilLoop:
            ret.next = buildBeginRepeatUntilLoop(ins);
            break;

        case CppInstruction::Type::BeginLoopCondition:
            ret.next = buildBeginLoopCondition(ins);
            break;

        case CppInstruction::Type::EndLoop:
            ret.next = buildEndLoop(ins);
            break;

        case CppInstruction::Type::Stop:
            ret.next = buildStop(ins);
            break;

        case CppInstruction::Type::ThreadStop:
            ret.next = buildThreadStop(ins);
            break;

        case CppInstruction::Type::InvalidateTarget:
            ret.next = buildInvalidateTarget(ins);
            break;

        case CppInstruction::Type::Print:
            ret.next = buildPrint(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Control::buildSelect(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildYield(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildBeginIf(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildBeginElse(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildEndIf(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildBeginRepeatLoop(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildLoopIndex(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildBeginWhileLoop(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildBeginRepeatUntilLoop(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildBeginLoopCondition(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildEndLoop(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildStop(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildThreadStop(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildInvalidateTarget(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Control::buildPrint(CppInstruction *ins)
{
    return ins->getNext();
}
