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

TEST(CppCodeGeneratorTest, ReporterCanBeConvertedAndBoxedWithoutChangingItsOriginalValue)
{
    CppCodeGenerator codeGen;
    CppValue numberResult(Compiler::StaticType::Number);
    numberResult.isRawValue = true;

    codeGen.createCall("test_get_number", CppIdentifier::Type::Double, &numberResult, false, false, {});

    const CppIdentifier *boolArg = codeGen.castValue(&numberResult, Compiler::StaticType::Bool);
    codeGen.createCall("test_print_bool", false, false, { boolArg });

    const CppIdentifier *numberArg = codeGen.castValue(&numberResult, Compiler::StaticType::Number);
    codeGen.createCall("test_print_number", false, false, { numberArg });

    const CppIdentifier *valueArg = codeGen.castValue(&numberResult, Compiler::StaticType::Unknown);
    codeGen.createCall("test_print_unknown", false, false, { valueArg });

    const std::string source = codeGen.finalize();
    EXPECT_NE(source.find("test_print_bool(castRawValue0)"), std::string::npos);
    EXPECT_NE(source.find("test_print_number(ret0)"), std::string::npos);
    EXPECT_NE(source.find("value0.numberValue = ret0;"), std::string::npos) << source;
    EXPECT_NE(source.find("value0.type = ValueType::Number;"), std::string::npos) << source;
    EXPECT_NE(source.find("test_print_unknown(value0)"), std::string::npos) << source;
}
