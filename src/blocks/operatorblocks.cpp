// SPDX-License-Identifier: Apache-2.0

#include "operatorblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

OperatorBlocks::OperatorBlocks()
{
    // Blocks
    addCompileFunction("operator_add", &OperatorBlocks::compileAdd);
    addCompileFunction("operator_subtract", &OperatorBlocks::compileSubtract);
    addCompileFunction("operator_multiply", &OperatorBlocks::compileMultiply);
    addCompileFunction("operator_divide", &OperatorBlocks::compileDivide);
    addCompileFunction("operator_random", &OperatorBlocks::compilePickRandom);
    addCompileFunction("operator_lt", &OperatorBlocks::compileLessThan);
    addCompileFunction("operator_equals", &OperatorBlocks::compileEquals);
    addCompileFunction("operator_gt", &OperatorBlocks::compileGreaterThan);
    addCompileFunction("operator_and", &OperatorBlocks::compileAnd);
    addCompileFunction("operator_or", &OperatorBlocks::compileOr);
    addCompileFunction("operator_not", &OperatorBlocks::compileNot);
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

void OperatorBlocks::compileAdd(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_ADD);
}

void OperatorBlocks::compileSubtract(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_SUBTRACT);
}

void OperatorBlocks::compileMultiply(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_MULTIPLY);
}

void OperatorBlocks::compileDivide(Compiler *compiler)
{
    compiler->addInput(NUM1);
    compiler->addInput(NUM2);
    compiler->addInstruction(vm::OP_DIVIDE);
}

void OperatorBlocks::compilePickRandom(Compiler *compiler)
{
    compiler->addInput(FROM);
    compiler->addInput(TO);
    compiler->addInstruction(vm::OP_RANDOM);
}

void OperatorBlocks::compileLessThan(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_LESS_THAN);
}

void OperatorBlocks::compileEquals(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_EQUALS);
}

void OperatorBlocks::compileGreaterThan(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_GREATER_THAN);
}

void OperatorBlocks::compileAnd(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_AND);
}

void OperatorBlocks::compileOr(Compiler *compiler)
{
    compiler->addInput(OPERAND1);
    compiler->addInput(OPERAND2);
    compiler->addInstruction(vm::OP_OR);
}

void OperatorBlocks::compileNot(Compiler *compiler)
{
    compiler->addInput(OPERAND);
    compiler->addInstruction(vm::OP_NOT);
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
    return randint<double>(args.input(FROM)->value().toDouble(), args.input(TO)->value().toDouble());
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
