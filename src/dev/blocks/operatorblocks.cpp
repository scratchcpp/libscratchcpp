// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilervalue.h>

#include "operatorblocks.h"

using namespace libscratchcpp;

std::string OperatorBlocks::name() const
{
    return "Operators";
}

std::string OperatorBlocks::description() const
{
    return "Operator blocks";
}

void OperatorBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "operator_add", &compileAdd);
    engine->addCompileFunction(this, "operator_subtract", &compileSubtract);
    engine->addCompileFunction(this, "operator_multiply", &compileMultiply);
    engine->addCompileFunction(this, "operator_divide", &compileDivide);
    engine->addCompileFunction(this, "operator_random", &compileRandom);
    engine->addCompileFunction(this, "operator_lt", &compileLt);
    engine->addCompileFunction(this, "operator_equals", &compileEquals);
    engine->addCompileFunction(this, "operator_gt", &compileGt);
    engine->addCompileFunction(this, "operator_and", &compileAnd);
}

CompilerValue *OperatorBlocks::compileAdd(Compiler *compiler)
{
    return compiler->createAdd(compiler->addInput("NUM1"), compiler->addInput("NUM2"));
}

CompilerValue *OperatorBlocks::compileSubtract(Compiler *compiler)
{
    return compiler->createSub(compiler->addInput("NUM1"), compiler->addInput("NUM2"));
}

CompilerValue *OperatorBlocks::compileMultiply(Compiler *compiler)
{
    return compiler->createMul(compiler->addInput("NUM1"), compiler->addInput("NUM2"));
}

CompilerValue *OperatorBlocks::compileDivide(Compiler *compiler)
{
    return compiler->createDiv(compiler->addInput("NUM1"), compiler->addInput("NUM2"));
}

CompilerValue *OperatorBlocks::compileRandom(Compiler *compiler)
{
    auto from = compiler->addInput("FROM");
    auto to = compiler->addInput("TO");
    return compiler->createRandom(from, to);
}

CompilerValue *OperatorBlocks::compileLt(Compiler *compiler)
{
    return compiler->createCmpLT(compiler->addInput("OPERAND1"), compiler->addInput("OPERAND2"));
}

CompilerValue *OperatorBlocks::compileEquals(Compiler *compiler)
{
    return compiler->createCmpEQ(compiler->addInput("OPERAND1"), compiler->addInput("OPERAND2"));
}

CompilerValue *OperatorBlocks::compileGt(Compiler *compiler)
{
    return compiler->createCmpGT(compiler->addInput("OPERAND1"), compiler->addInput("OPERAND2"));
}

CompilerValue *OperatorBlocks::compileAnd(Compiler *compiler)
{
    return compiler->createAnd(compiler->addInput("OPERAND1"), compiler->addInput("OPERAND2"));
}
