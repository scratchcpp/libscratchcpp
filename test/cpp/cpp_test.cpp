// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/thread.h>
#include <scratchcpp/value.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/compilerconstant.h>
#include <engine/internal/cpp/cppcodebuilder.h>
#include <engine/internal/cpp/cppcompilercontext.h>
#include <engine/internal/cpp/cppexecutablecode.h>
#include <engine/internal/cpp/cppcodebasegenerator.h>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "cases.h"

using namespace libscratchcpp;

class CppCodeBuilderTest : public testing::TestWithParam<CppTestCase>
{ };

static std::string caseName(const testing::TestParamInfo<CppTestCase> &info)
{
    return info.param.name;
}

TEST_P(CppCodeBuilderTest, Run)
{
    const CppTestCase &testCase = GetParam();
    CppTestEnv env;
    auto code = testCase.build(env);
    ASSERT_TRUE(code);
    ASSERT_TRUE(std::static_pointer_cast<CppExecutableCode>(code)->isResolved()) << "the generated code is out of date";

    Thread thread(&env.sprite, nullptr, nullptr);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();

    switch (testCase.codeType) {
        case Compiler::CodeType::Script: {
            int runs = 0;

            while (!code->isFinished(ctx.get()) && runs < 1000) {
                code->run(ctx.get());
                runs++;
            }

            ASSERT_TRUE(code->isFinished(ctx.get()));

            if (testCase.expectedRuns != -1)
                ASSERT_EQ(runs, testCase.expectedRuns);

            break;
        }

        case Compiler::CodeType::Reporter: {
            ValueData ret = code->runReporter(ctx.get());
            Value v(ret);
            value_free(&ret);
            ASSERT_EQ(v.toString(), testCase.expectedValue);
            break;
        }

        case Compiler::CodeType::HatPredicate:
            ASSERT_EQ(Value(code->runPredicate(ctx.get())).toString(), testCase.expectedValue);
            break;
    }

    ASSERT_EQ(testing::internal::GetCapturedStdout(), testCase.expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(Generated, CppCodeBuilderTest, testing::ValuesIn(cppTestCases()), caseName);

static const CppTestCase &findCase(const std::string &name)
{
    for (const CppTestCase &testCase : cppTestCases()) {
        if (testCase.name == name)
            return testCase;
    }

    assert(false);
    return cppTestCases()[0];
}

TEST(CppExecutableCodeTest, PromiseKillReset)
{
    CppTestEnv env;
    auto code = findCase("yield").build(env);
    Thread thread(&env.sprite, nullptr, nullptr);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();

    code->run(ctx.get());
    ASSERT_FALSE(code->isFinished(ctx.get()));

    // Unresolved promise blocks the script
    auto promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    code->run(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n");

    testing::internal::CaptureStdout();
    promise->resolve();
    code->run(ctx.get());
    ASSERT_EQ(ctx->promise(), nullptr);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "2\n");

    // Reset destroys the suspended frame
    code->reset(ctx.get());
    ASSERT_FALSE(code->isFinished(ctx.get()));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "1\n");

    // Kill finishes the script
    code->kill(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST(CppCompilerContextTest, ModuleSource)
{
    CppTestEnv env;
    auto b = env.createBuilder();
    b->createPrint(b->addConstValue("test"));
    b->build();

    std::string source = env.ctx->moduleSource();
    ASSERT_NE(source.find(env.ctx->sourceCode().substr(0, 20)), std::string::npos);
    ASSERT_NE(source.find("cpp_register_function"), std::string::npos);
    ASSERT_EQ(env.ctx->moduleKey(), CppCompilerContext::hash(env.ctx->sourceCode()));
    ASSERT_NE(CppCompilerContext::prelude().find("struct Frame"), std::string::npos);
}

TEST(CppExecutableCodeTest, Unresolved)
{
    CppTestEnv env;
    auto b = env.createBuilder();
    b->createPrint(b->addConstValue("this code is not compiled"));
    auto code = std::static_pointer_cast<CppExecutableCode>(b->build());
    ASSERT_FALSE(code->isResolved());
    ASSERT_EQ(code->functionName(), "script0");
}

TEST(CppCodebaseGeneratorTest, Generate)
{
    std::filesystem::path dir = std::filesystem::temp_directory_path() / "scratchcpp_cpp_codebase_test";
    std::filesystem::remove_all(dir);
    ASSERT_TRUE(CppCodebaseGenerator::generate(DATA_DIR "/custom_blocks.sb3", dir.string(), "my project"));

    auto read = [&dir](const std::string &name) {
        std::ifstream file(dir / name);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    };

    std::string code = read("project.cpp");
    ASSERT_NE(code.find("struct Frame"), std::string::npos);
    ASSERT_NE(code.find("cpp_register_function"), std::string::npos);
    ASSERT_NE(code.find("proc_"), std::string::npos);
    ASSERT_NE(read("main.cpp").find("cpp_use_cpp_backend()"), std::string::npos);
    ASSERT_NE(read("project_data.cpp").find("scratchcpp_project_data[]"), std::string::npos);
    ASSERT_NE(read("CMakeLists.txt").find("add_executable(my_project main.cpp project.cpp project_data.cpp)"), std::string::npos);

    ASSERT_FALSE(CppCodebaseGenerator::generate("invalid.sb3", dir.string()));
    std::filesystem::remove_all(dir);
}

TEST(CppCodebaseGeneratorTest, DataSource)
{
    ASSERT_EQ(
        CppCodebaseGenerator::dataSource("ab"),
        "// Generated by libscratchcpp, do not edit\n\nextern const unsigned long long scratchcpp_project_size = 2ULL;\n\nextern const unsigned char scratchcpp_project_data[] = {\n    0x61, 0x62,\n  "
        "  0x00\n};\n");
}
