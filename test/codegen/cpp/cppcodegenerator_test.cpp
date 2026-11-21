// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include <vector>

#include <engine/internal/codegen/cpp/cppcodegenerator.h>
#include <engine/internal/codegen/cpp/cppcodesaver.h>
#include <engine/internal/codegen/cpp/cppcompilercontext.h>
#include <engine/internal/codegen/cpp/cppvalue.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>
#include <enginemock.h>

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

TEST(CppCodeSaverTest, WritesOnePairOfFilesPerScriptInTargetDirectories)
{
    const std::filesystem::path destination = std::filesystem::temp_directory_path() / "libscratchcpp-cpp-code-saver-test";
    std::filesystem::remove_all(destination);

    EngineMock engine;
    auto stage = std::make_shared<Stage>();
    auto sprite0 = std::make_shared<Sprite>();
    auto sprite1 = std::make_shared<Sprite>();
    const std::vector<std::shared_ptr<Target>> targets = { stage, sprite0, sprite1 };
    EXPECT_CALL(engine, targets()).WillRepeatedly(testing::ReturnRef(targets));

    CppCompilerContext stageContext(&engine, stage.get());
    CppCompilerContext sprite0Context(&engine, sprite0.get());
    CppCompilerContext sprite1Context(&engine, sprite1.get());
    CppCodeGenerator stageCode;
    CppCodeGenerator sprite0Code;
    CppCodeGenerator sprite1Code;
    const std::string stageFileName = stageCode.generatedFileName();
    const std::string sprite0FileName = sprite0Code.generatedFileName();
    const std::string sprite1FileName = sprite1Code.generatedFileName();

    CppCodeSaver::save(stageCode, &stageContext, destination.string());
    CppCodeSaver::save(sprite0Code, &sprite0Context, destination.string());
    CppCodeSaver::save(sprite1Code, &sprite1Context, destination.string());

    EXPECT_TRUE(std::filesystem::exists(destination / "src/stage" / (stageFileName + ".cpp")));
    EXPECT_TRUE(std::filesystem::exists(destination / "src/stage" / (stageFileName + ".h")));
    EXPECT_TRUE(std::filesystem::exists(destination / "src/sprites/sprite0" / (sprite0FileName + ".cpp")));
    EXPECT_TRUE(std::filesystem::exists(destination / "src/sprites/sprite0" / (sprite0FileName + ".h")));
    EXPECT_TRUE(std::filesystem::exists(destination / "src/sprites/sprite1" / (sprite1FileName + ".cpp")));
    EXPECT_TRUE(std::filesystem::exists(destination / "src/sprites/sprite1" / (sprite1FileName + ".h")));

    std::filesystem::remove_all(destination);
}
