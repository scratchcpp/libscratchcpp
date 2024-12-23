// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilervalue.h>
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

CompilerValue *OperatorBlocks::compileLetterOf(Compiler *compiler)
{
    auto letter = compiler->addInput("LETTER");
    auto string = compiler->addInput("STRING");
    return compiler->addFunctionCall("operator_letter_of", Compiler::StaticType::String, { Compiler::StaticType::Number, Compiler::StaticType::String }, { letter, string });
}

CompilerValue *OperatorBlocks::compileLength(Compiler *compiler)
{
    auto string = compiler->addInput("STRING");
    return compiler->addFunctionCall("operator_length", Compiler::StaticType::Number, { Compiler::StaticType::String }, { string });
}

CompilerValue *OperatorBlocks::compileContains(Compiler *compiler)
{
    auto string1 = compiler->addInput("STRING1");
    auto string2 = compiler->addInput("STRING2");
    return compiler->addFunctionCall("operator_contains", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { string1, string2 });
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

extern "C" char *operator_letter_of(double letter, const char *string)
{
    const size_t len = strlen(string);

    if (letter < 1 || letter > len) {
        char *ret = (char *)malloc(sizeof(char));
        ret[0] = '\0';
        return ret;
    }

    // TODO: Rewrite this
    std::u16string u16 = utf8::utf8to16(std::string(string));
    std::string str = utf8::utf16to8(std::u16string({ u16[(size_t)letter - 1] }));
    char *ret = (char *)malloc((str.size() + 1) * sizeof(char));
    strcpy(ret, str.c_str());

    return ret;
}

extern "C" double operator_length(const char *string)
{
    // TODO: Rewrite this
    return utf8::utf8to16(std::string(string)).size();
}

extern "C" bool operator_contains(const char *string1, const char *string2)
{
    // TODO: Rewrite this
    std::u16string u16string1 = utf8::utf8to16(std::string(string1));
    std::u16string u16string2 = utf8::utf8to16(std::string(string2));
    std::transform(u16string1.begin(), u16string1.end(), u16string1.begin(), ::tolower);
    std::transform(u16string2.begin(), u16string2.end(), u16string2.begin(), ::tolower);
    return (u16string1.find(u16string2) != std::u16string::npos);
}
