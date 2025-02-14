// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/field.h>
#include <scratchcpp/value.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>
#include <utf8.h>

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

Rgb OperatorBlocks::color() const
{
    return rgb(89, 192, 89);
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
    engine->addCompileFunction(this, "operator_letter_of", &compileLetterOf);
    engine->addCompileFunction(this, "operator_length", &compileLength);
    engine->addCompileFunction(this, "operator_contains", &compileContains);
    engine->addCompileFunction(this, "operator_mod", &compileMod);
    engine->addCompileFunction(this, "operator_round", &compileRound);
    engine->addCompileFunction(this, "operator_mathop", &compileMathOp);
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
    return compiler->createStringConcat(string1, string2);
}

CompilerValue *OperatorBlocks::compileLetterOf(Compiler *compiler)
{
    auto letter = compiler->addInput("LETTER");
    auto string = compiler->addInput("STRING");
    return compiler->addStringChar(string, compiler->createSub(letter, compiler->addConstValue(1)));
}

CompilerValue *OperatorBlocks::compileLength(Compiler *compiler)
{
    auto string = compiler->addInput("STRING");
    return compiler->addStringLength(string);
}

CompilerValue *OperatorBlocks::compileContains(Compiler *compiler)
{
    auto string1 = compiler->addInput("STRING1");
    auto string2 = compiler->addInput("STRING2");
    return compiler->addFunctionCall("string_contains_case_insensitive", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { string1, string2 });
}

CompilerValue *OperatorBlocks::compileMod(Compiler *compiler)
{
    return compiler->createMod(compiler->addInput("NUM1"), compiler->addInput("NUM2"));
}

CompilerValue *OperatorBlocks::compileRound(Compiler *compiler)
{
    return compiler->createRound(compiler->addInput("NUM"));
}

CompilerValue *OperatorBlocks::compileMathOp(Compiler *compiler)
{
    Field *opField = compiler->field("OPERATOR");
    const std::string numInput = "NUM";
    const std::string &op = opField->value().toString();

    if (op == "abs")
        return compiler->createAbs(compiler->addInput(numInput));
    else if (op == "floor")
        return compiler->createFloor(compiler->addInput(numInput));
    else if (op == "ceiling")
        return compiler->createCeil(compiler->addInput(numInput));
    else if (op == "sqrt")
        return compiler->createSqrt(compiler->addInput(numInput));
    else if (op == "sin")
        return compiler->createSin(compiler->addInput(numInput));
    else if (op == "cos")
        return compiler->createCos(compiler->addInput(numInput));
    else if (op == "tan")
        return compiler->createTan(compiler->addInput(numInput));
    else if (op == "asin")
        return compiler->createAsin(compiler->addInput(numInput));
    else if (op == "acos")
        return compiler->createAcos(compiler->addInput(numInput));
    else if (op == "atan")
        return compiler->createAtan(compiler->addInput(numInput));
    else if (op == "ln")
        return compiler->createLn(compiler->addInput(numInput));
    else if (op == "log")
        return compiler->createLog10(compiler->addInput(numInput));
    else if (op == "e ^")
        return compiler->createExp(compiler->addInput(numInput));
    else if (op == "10 ^")
        return compiler->createExp10(compiler->addInput(numInput));
    else
        return compiler->addConstValue(Value());
}
