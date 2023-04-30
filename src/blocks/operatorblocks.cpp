// SPDX-License-Identifier: Apache-2.0

#include "operatorblocks.h"

using namespace libscratchcpp;

OperatorBlocks::OperatorBlocks()
{
    // Blocks
    addBlock("operator_add", &OperatorBlocks::add);
    addBlock("operator_subtract", &OperatorBlocks::subtract);
    addBlock("operator_multiply", &OperatorBlocks::multiply);
    addBlock("operator_divide", &OperatorBlocks::divide);
    addBlock("operator_random", &OperatorBlocks::pickRandom);
    addBlock("operator_lt", &OperatorBlocks::lessThan);
    addBlock("operator_equals", &OperatorBlocks::equals);
    addBlock("operator_gt", &OperatorBlocks::greaterThan);
    addBlock("operator_and", &OperatorBlocks::andGate);
    addBlock("operator_or", &OperatorBlocks::orGate);
    addBlock("operator_not", &OperatorBlocks::notGate);

    // Inputs
    addInput("NUM1", NUM1);
    addInput("NUM2", NUM2);
    addInput("FROM", FROM);
    addInput("TO", TO);
    addInput("OPERAND1", OPERAND1);
    addInput("OPERAND2", OPERAND2);
    addInput("OPERAND", OPERAND);
}

std::string OperatorBlocks::name() const
{
    return "Operators";
}

Value OperatorBlocks::add(const BlockArgs &args)
{
    return args.input(NUM1)->value() + args.input(NUM2)->value();
}

Value OperatorBlocks::subtract(const BlockArgs &args)
{
    return args.input(NUM1)->value() - args.input(NUM2)->value();
}

Value OperatorBlocks::multiply(const BlockArgs &args)
{
    return args.input(NUM1)->value() * args.input(NUM2)->value();
}

Value OperatorBlocks::divide(const BlockArgs &args)
{
    return args.input(NUM1)->value() / args.input(NUM2)->value();
}

Value OperatorBlocks::pickRandom(const BlockArgs &args)
{
    return randint(args.input(FROM)->value().toDouble(), args.input(TO)->value().toDouble());
}

Value OperatorBlocks::lessThan(const BlockArgs &args)
{
    return args.input(OPERAND1)->value() < args.input(OPERAND2)->value();
}

Value OperatorBlocks::equals(const BlockArgs &args)
{
    return args.input(OPERAND1)->value() == args.input(OPERAND2)->value();
}

Value OperatorBlocks::greaterThan(const BlockArgs &args)
{
    return args.input(OPERAND1)->value() > args.input(OPERAND2)->value();
}

Value OperatorBlocks::andGate(const BlockArgs &args)
{
    return args.input(OPERAND1)->value().toBool() && args.input(OPERAND2)->value().toBool();
}

Value OperatorBlocks::orGate(const BlockArgs &args)
{
    return args.input(OPERAND1)->value().toBool() || args.input(OPERAND2)->value().toBool();
}

Value OperatorBlocks::notGate(const BlockArgs &args)
{
    return !args.input(OPERAND)->value().toBool();
}
