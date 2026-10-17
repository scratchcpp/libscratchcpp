// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/thread.h>
#include <scratchcpp/value.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/compilerconstant.h>
#include <engine/internal/cpp/cppcodebuilder.h>
#include <engine/internal/cpp/cppcompilercontext.h>
#include <engine/internal/cpp/cppexecutablecode.h>
#include <gtest/gtest.h>

#include "cases.h"

using namespace libscratchcpp;

class CppCodeBuilderTest : public testing::TestWithParam<CppTestCase>
{
};

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
