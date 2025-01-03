#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/value.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <gtest/gtest.h>
#include <iostream>

#include "testextension.h"

using namespace libscratchcpp;

std::string TestExtension::name() const
{
    return "test";
}

std::string TestExtension::description() const
{
    return "";
}

void TestExtension::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "test_simple", &compileSimple);
    engine->addCompileFunction(this, "test_print", &compilePrint);
    engine->addCompileFunction(this, "test_print_dropdown", &compilePrintDropdown);
    engine->addCompileFunction(this, "test_print_field", &compilePrintField);
    engine->addCompileFunction(this, "test_teststr", &compileTestStr);
    engine->addCompileFunction(this, "test_input", &compileInput);
    engine->addCompileFunction(this, "test_substack", &compileSubstack);
    engine->addCompileFunction(this, "test_click_hat", &compileClickHat);
}

CompilerValue *TestExtension::compileSimple(Compiler *compiler)
{
    compiler->addFunctionCall("test_simple", Compiler::StaticType::Void);
    return nullptr;
}

CompilerValue *TestExtension::compilePrint(Compiler *compiler)
{
    CompilerValue *input = compiler->addInput("STRING");
    compiler->addFunctionCall("test_print", Compiler::StaticType::Void, { Compiler::StaticType::String }, { input });
    return nullptr;
}

CompilerValue *TestExtension::compilePrintDropdown(Compiler *compiler)
{
    EXPECT_TRUE(compiler->input("STRING")->pointsToDropdownMenu());
    CompilerValue *input = compiler->addInput("STRING");
    compiler->addFunctionCall("test_print", Compiler::StaticType::Void, { Compiler::StaticType::String }, { input });
    return nullptr;
}

CompilerValue *TestExtension::compilePrintField(Compiler *compiler)
{
    CompilerValue *input = compiler->addConstValue(compiler->field("STRING")->value());
    compiler->addFunctionCall("test_print", Compiler::StaticType::Void, { Compiler::StaticType::String }, { input });
    return nullptr;
}

CompilerValue *TestExtension::compileTestStr(Compiler *compiler)
{
    return compiler->addConstValue("test");
}

CompilerValue *TestExtension::compileInput(Compiler *compiler)
{
    return compiler->addInput("INPUT");
}

CompilerValue *TestExtension::compileSubstack(Compiler *compiler)
{
    auto substack = compiler->input("SUBSTACK");
    compiler->moveToIf(compiler->addConstValue(true), substack->valueBlock());
    return nullptr;
}

CompilerValue *TestExtension::compileClickHat(Compiler *compiler)
{
    compiler->engine()->addTargetClickScript(compiler->block());
    return nullptr;
}

extern "C" void test_simple()
{
    std::cout << "test" << std::endl;
}

extern "C" void test_print(const char *string)
{
    std::cout << string << std::endl;
}
