#include <scratchcpp/value.h>
#include <scratchcpp/dev/executablecode.h>
#include <dev/engine/internal/llvmcodebuilder.h>
#include <gmock/gmock.h>
#include <targetmock.h>

#include "testfunctions.h"

using namespace libscratchcpp;

using ::testing::Return;

class LLVMCodeBuilderTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_builder = std::make_unique<LLVMCodeBuilder>("test");
            test_function(nullptr, nullptr); // force dependency
        }

        std::unique_ptr<LLVMCodeBuilder> m_builder;
        TargetMock m_target; // NOTE: isStage() is used for call expectations
};

TEST_F(LLVMCodeBuilderTest, FunctionCalls)
{
    m_builder->addFunctionCall("test_function_no_args", 0, false);

    m_builder->addFunctionCall("test_function_no_args_ret", 0, true);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);

    m_builder->addConstValue(1);
    m_builder->addFunctionCall("test_function_1_arg_ret", 1, true);
    m_builder->addConstValue(2);
    m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_function_3_args", 3, false);

    m_builder->addConstValue("test");
    m_builder->addConstValue(4);
    m_builder->addConstValue(5);
    m_builder->addFunctionCall("test_function_3_args_ret", 3, true);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected =
        "no_args\n"
        "no_args_ret\n"
        "1_arg no_args_output\n"
        "1_arg_ret 1\n"
        "3_args 1_arg_output 2 3\n"
        "3_args test 4 5\n"
        "1_arg 3_args_output\n";

    EXPECT_CALL(m_target, isStage()).Times(7);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, Yield)
{
    m_builder->addFunctionCall("test_function_no_args", 0, false);

    m_builder->addFunctionCall("test_function_no_args_ret", 0, true);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);

    m_builder->yield();

    m_builder->addConstValue(1);
    m_builder->addFunctionCall("test_function_1_arg_ret", 1, true);
    m_builder->addConstValue(2);
    m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_function_3_args", 3, false);

    m_builder->addConstValue("test");
    m_builder->addConstValue(4);
    m_builder->addConstValue(5);
    m_builder->addFunctionCall("test_function_3_args_ret", 3, true);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected1 =
        "no_args\n"
        "no_args_ret\n"
        "1_arg no_args_output\n";

    EXPECT_CALL(m_target, isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    static const std::string expected2 =
        "1_arg_ret 1\n"
        "3_args 1_arg_output 2 3\n"
        "3_args test 4 5\n"
        "1_arg 3_args_output\n";

    EXPECT_CALL(m_target, isStage()).Times(4);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected2);
    ASSERT_TRUE(code->isFinished(ctx.get()));
}

TEST_F(LLVMCodeBuilderTest, IfStatement)
{
    // Without else branch (const condition)
    m_builder->addConstValue("true");
    m_builder->beginIfStatement();
    m_builder->addFunctionCall("test_function_no_args", 0, false);
    m_builder->endIf();

    m_builder->addConstValue("false");
    m_builder->beginIfStatement();
    m_builder->addFunctionCall("test_function_no_args", 0, false);
    m_builder->endIf();

    // Without else branch (condition returned by function)
    m_builder->addFunctionCall("test_function_no_args_ret", 0, true);
    m_builder->addConstValue("no_args_output");
    m_builder->addFunctionCall("test_equals", 2, true);
    m_builder->beginIfStatement();
    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->endIf();

    m_builder->addFunctionCall("test_function_no_args_ret", 0, true);
    m_builder->addConstValue("");
    m_builder->addFunctionCall("test_equals", 2, true);
    m_builder->beginIfStatement();
    m_builder->addConstValue(1);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->endIf();

    // With else branch (const condition)
    m_builder->addConstValue("true");
    m_builder->beginIfStatement();
    m_builder->addConstValue(2);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->beginElseBranch();
    m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->endIf();

    m_builder->addConstValue("false");
    m_builder->beginIfStatement();
    m_builder->addConstValue(4);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->beginElseBranch();
    m_builder->addConstValue(5);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->endIf();

    // With else branch (condition returned by function)
    m_builder->addFunctionCall("test_function_no_args_ret", 0, true);
    m_builder->addConstValue("no_args_output");
    m_builder->addFunctionCall("test_equals", 2, true);
    m_builder->beginIfStatement();
    m_builder->addConstValue(6);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->beginElseBranch();
    m_builder->addConstValue(7);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->endIf();

    m_builder->addFunctionCall("test_function_no_args_ret", 0, true);
    m_builder->addConstValue("");
    m_builder->addFunctionCall("test_equals", 2, true);
    m_builder->beginIfStatement();
    m_builder->addConstValue(8);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->beginElseBranch();
    m_builder->addConstValue(9);
    m_builder->addFunctionCall("test_function_1_arg", 1, false);
    m_builder->endIf();

    // Nested 1
    m_builder->addConstValue(true);
    m_builder->beginIfStatement();
    {
        m_builder->addConstValue(false);
        m_builder->beginIfStatement();
        {
            m_builder->addConstValue(0);
            m_builder->addFunctionCall("test_function_1_arg", 1, false);
        }
        m_builder->beginElseBranch();
        {
            m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", 1, false);

            m_builder->addConstValue(false);
            m_builder->beginIfStatement();
            m_builder->beginElseBranch();
            {
                m_builder->addConstValue(2);
                m_builder->addFunctionCall("test_function_1_arg", 1, false);
            }
            m_builder->endIf();
        }
        m_builder->endIf();
    }
    m_builder->beginElseBranch();
    {
        m_builder->addConstValue(true);
        m_builder->beginIfStatement();
        {
            m_builder->addConstValue(3);
            m_builder->addFunctionCall("test_function_1_arg", 1, false);
        }
        m_builder->beginElseBranch();
        {
            m_builder->addConstValue(4);
            m_builder->addFunctionCall("test_function_1_arg", 1, false);
        }
        m_builder->endIf();
    }
    m_builder->endIf();

    // Nested 2
    m_builder->addConstValue(false);
    m_builder->beginIfStatement();
    {
        m_builder->addConstValue(false);
        m_builder->beginIfStatement();
        {
            m_builder->addConstValue(5);
            m_builder->addFunctionCall("test_function_1_arg", 1, false);
        }
        m_builder->beginElseBranch();
        {
            m_builder->addConstValue(6);
            m_builder->addFunctionCall("test_function_1_arg", 1, false);
        }
        m_builder->endIf();
    }
    m_builder->beginElseBranch();
    {
        m_builder->addConstValue(true);
        m_builder->beginIfStatement();
        {
            m_builder->addConstValue(7);
            m_builder->addFunctionCall("test_function_1_arg", 1, false);
        }
        m_builder->beginElseBranch();
        m_builder->endIf();
    }
    m_builder->endIf();

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected =
        "no_args\n"
        "no_args_ret\n"
        "1_arg 0\n"
        "no_args_ret\n"
        "1_arg 2\n"
        "1_arg 5\n"
        "no_args_ret\n"
        "1_arg 6\n"
        "no_args_ret\n"
        "1_arg 9\n"
        "1_arg 1\n"
        "1_arg 2\n"
        "1_arg 7\n";

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}
