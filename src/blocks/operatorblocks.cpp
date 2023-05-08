// SPDX-License-Identifier: Apache-2.0

#include "operatorblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

OperatorBlocks::OperatorBlocks()
{
    // Blocks
    addCompileFunction("operator_add", &compileAdd);
    addCompileFunction("operator_subtract", &compileSubtract);
    addCompileFunction("operator_multiply", &compileMultiply);
    addCompileFunction("operator_divide", &compileDivide);
    addCompileFunction("operator_random", &compilePickRandom);
    addCompileFunction("operator_lt", &compileLessThan);
    addCompileFunction("operator_equals", &compileEquals);
    addCompileFunction("operator_gt", &compileGreaterThan);
    addCompileFunction("operator_and", &compileAnd);
    addCompileFunction("operator_or", &compileOr);
    addCompileFunction("operator_not", &compileNot);

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
