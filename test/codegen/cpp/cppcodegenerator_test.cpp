// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <engine/internal/codegen/cpp/cppcodegenerator.h>
#include <engine/internal/codegen/cpp/cppvalue.h>

using namespace libscratchcpp;

TEST(CppCodeGeneratorTest, StringReturnValueCanBePassedToAnotherCall)
{
    CppCodeGenerator codeGen;
    CppValue stringResult(Compiler::StaticType::String);
    stringResult.isRawValue = true;

    codeGen.createCall("test_function_no_args_ret", CppIdentifier::Type::StringPtr, &stringResult, false, false, {});

    const CppIdentifier *stringArg = codeGen.castValue(&stringResult, Compiler::StaticType::String);
    codeGen.createCall("test_print_string", false, false, { stringArg });

    const std::string source = codeGen.finalize();
    EXPECT_NE(source.find("test_print_string(string0)"), std::string::npos);
}
