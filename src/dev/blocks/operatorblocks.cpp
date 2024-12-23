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
    engine->addCompileFunction(this, "operator_or", &compileOr);
    engine->addCompileFunction(this, "operator_not", &compileNot);
    engine->addCompileFunction(this, "operator_join", &compileJoin);
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

CompilerValue *OperatorBlocks::compileOr(Compiler *compiler)
{
    return compiler->createOr(compiler->addInput("OPERAND1"), compiler->addInput("OPERAND2"));
}

CompilerValue *OperatorBlocks::compileNot(Compiler *compiler)
{
    return compiler->createNot(compiler->addInput("OPERAND"));
}

CompilerValue *OperatorBlocks::compileJoin(Compiler *compiler)
{
    auto string1 = compiler->addInput("STRING1");
    auto string2 = compiler->addInput("STRING2");
    return compiler->addFunctionCall("operator_join", Compiler::StaticType::String, { Compiler::StaticType::String, Compiler::StaticType::String }, { string1, string2 });
}

extern "C" char *operator_join(const char *string1, const char *string2)
{
    const size_t len1 = strlen(string1);
    const size_t len2 = strlen(string2);

    char *ret = (char *)malloc((len1 + len2 + 1) * sizeof(char));
    size_t i;

    for (i = 0; i < len1; i++)
        ret[i] = string1[i];

    for (i = 0; i < len2 + 1; i++) // +1: null-terminate
        ret[len1 + i] = string2[i];

    return ret;
}
