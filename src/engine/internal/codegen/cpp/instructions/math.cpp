// SPDX-License-Identifier: Apache-2.0

#include "math.h"
#include "../cppinstruction.h"
using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Math::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::Add:
            ret.next = buildAdd(ins);
            break;

        case CppInstruction::Type::Sub:
            ret.next = buildSub(ins);
            break;

        case CppInstruction::Type::Mul:
            ret.next = buildMul(ins);
            break;

        case CppInstruction::Type::Div:
            ret.next = buildDiv(ins);
            break;

        case CppInstruction::Type::Random:
            ret.next = buildRandom(ins);
            break;

        case CppInstruction::Type::RandomInt:
            ret.next = buildRandomInt(ins);
            break;

        case CppInstruction::Type::Mod:
            ret.next = buildMod(ins);
            break;

        case CppInstruction::Type::Round:
            ret.next = buildRound(ins);
            break;

        case CppInstruction::Type::Abs:
            ret.next = buildAbs(ins);
            break;

        case CppInstruction::Type::Floor:
            ret.next = buildFloor(ins);
            break;

        case CppInstruction::Type::Ceil:
            ret.next = buildCeil(ins);
            break;

        case CppInstruction::Type::Sqrt:
            ret.next = buildSqrt(ins);
            break;

        case CppInstruction::Type::Sin:
            ret.next = buildSin(ins);
            break;

        case CppInstruction::Type::Cos:
            ret.next = buildCos(ins);
            break;

        case CppInstruction::Type::Tan:
            ret.next = buildTan(ins);
            break;

        case CppInstruction::Type::Asin:
            ret.next = buildAsin(ins);
            break;

        case CppInstruction::Type::Acos:
            ret.next = buildAcos(ins);
            break;

        case CppInstruction::Type::Atan:
            ret.next = buildAtan(ins);
            break;

        case CppInstruction::Type::Ln:
            ret.next = buildLn(ins);
            break;

        case CppInstruction::Type::Log10:
            ret.next = buildLog10(ins);
            break;

        case CppInstruction::Type::Exp:
            ret.next = buildExp(ins);
            break;

        case CppInstruction::Type::Exp10:
            ret.next = buildExp10(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Math::buildAdd(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildSub(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildMul(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildDiv(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildRandom(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildRandomInt(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildMod(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildRound(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildAbs(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildFloor(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildCeil(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildSqrt(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildSin(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildCos(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildTan(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildAsin(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildAcos(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildAtan(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildLn(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildLog10(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildExp(CppInstruction *ins)
{
    return ins->getNext();
}

CppInstruction *Math::buildExp10(CppInstruction *ins)
{
    return ins->getNext();
}
