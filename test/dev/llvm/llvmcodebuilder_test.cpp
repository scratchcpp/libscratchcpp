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
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});

    m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {});
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });

    m_builder->addConstValue("1");
    m_builder->addFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String });
    m_builder->addConstValue("2");
    m_builder->addConstValue("3");
    m_builder->addFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String });

    m_builder->addConstValue("test");
    m_builder->addConstValue("4");
    m_builder->addConstValue("5");
    m_builder->addFunctionCall("test_function_3_args_ret", Compiler::StaticType::String, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String });
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
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

TEST_F(LLVMCodeBuilderTest, ConstCasting)
{
    m_builder->addConstValue(5.2);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });
    m_builder->addConstValue("-24.156");
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

    m_builder->addConstValue(true);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    m_builder->addConstValue("false");
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    m_builder->addConstValue("123");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

    m_builder->addConstValue("hello world");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected =
        "5.2\n"
        "-24.156\n"
        "1\n"
        "0\n"
        "0\n"
        "123\n"
        "hello world\n";

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, RawValueCasting)
{
    // Number -> number
    m_builder->addConstValue(5.2);
    m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

    // Number -> bool
    m_builder->addConstValue(-24.156);
    m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    // Number -> string
    m_builder->addConstValue(59.8);
    m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

    // Bool -> number
    m_builder->addConstValue(true);
    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

    m_builder->addConstValue(false);
    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

    // Bool -> bool
    m_builder->addConstValue(true);
    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    m_builder->addConstValue(false);
    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    // Bool -> string
    m_builder->addConstValue(true);
    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

    m_builder->addConstValue(false);
    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

    // String -> number
    m_builder->addConstValue("5.2");
    m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String });
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

    // String -> bool
    m_builder->addConstValue("abc");
    m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String });
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    m_builder->addConstValue("false");
    m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String });
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool });

    // String -> string
    m_builder->addConstValue("hello world");
    m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String });
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected =
        "5.2\n"
        "1\n"
        "0\n"
        "59.8\n"
        "1\n"
        "0\n"
        "1\n"
        "0\n"
        "true\n"
        "false\n"
        "5.2\n"
        "1\n"
        "0\n"
        "hello world\n";

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

/*TEST_F(LLVMCodeBuilderTest, Yield)
{
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});

    m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {});
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });

    m_builder->yield();

    m_builder->addConstValue("1");
    m_builder->addFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String });
    m_builder->addConstValue("2");
    m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String });

    m_builder->addConstValue("test");
    m_builder->addConstValue("4");
    m_builder->addConstValue("5");
    m_builder->addFunctionCall("test_function_3_args_ret", Compiler::StaticType::String, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String });
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
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
}*/

TEST_F(LLVMCodeBuilderTest, IfStatement)
{
    // Without else branch (const condition)
    m_builder->addConstValue("true");
    m_builder->beginIfStatement();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endIf();

    m_builder->addConstValue("false");
    m_builder->beginIfStatement();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endIf();

    // Without else branch (condition returned by function)
    m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {});
    m_builder->addConstValue("no_args_output");
    m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String });
    m_builder->beginIfStatement();
    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endIf();

    m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {});
    m_builder->addConstValue("");
    m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String });
    m_builder->beginIfStatement();
    m_builder->addConstValue(1);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endIf();

    // With else branch (const condition)
    m_builder->addConstValue("true");
    m_builder->beginIfStatement();
    m_builder->addConstValue(2);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->beginElseBranch();
    m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endIf();

    m_builder->addConstValue("false");
    m_builder->beginIfStatement();
    m_builder->addConstValue(4);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->beginElseBranch();
    m_builder->addConstValue(5);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endIf();

    // With else branch (condition returned by function)
    m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {});
    m_builder->addConstValue("no_args_output");
    m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String });
    m_builder->beginIfStatement();
    m_builder->addConstValue(6);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->beginElseBranch();
    m_builder->addConstValue(7);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endIf();

    m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {});
    m_builder->addConstValue("");
    m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String });
    m_builder->beginIfStatement();
    m_builder->addConstValue(8);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->beginElseBranch();
    m_builder->addConstValue(9);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endIf();

    // Nested 1
    m_builder->addConstValue(true);
    m_builder->beginIfStatement();
    {
        m_builder->addConstValue(false);
        m_builder->beginIfStatement();
        {
            m_builder->addConstValue(0);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
        }
        m_builder->beginElseBranch();
        {
            m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });

            m_builder->addConstValue(false);
            m_builder->beginIfStatement();
            m_builder->beginElseBranch();
            {
                m_builder->addConstValue(2);
                m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
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
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
        }
        m_builder->beginElseBranch();
        {
            m_builder->addConstValue(4);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
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
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
        }
        m_builder->beginElseBranch();
        {
            m_builder->addConstValue(6);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
        }
        m_builder->endIf();
    }
    m_builder->beginElseBranch();
    {
        m_builder->addConstValue(true);
        m_builder->beginIfStatement();
        {
            m_builder->addConstValue(7);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
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

TEST_F(LLVMCodeBuilderTest, RepeatLoop)
{
    // Const count
    m_builder->addConstValue("-5");
    m_builder->beginRepeatLoop();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    m_builder->addConstValue(0);
    m_builder->beginRepeatLoop();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    m_builder->addConstValue(3);
    m_builder->beginRepeatLoop();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    m_builder->addConstValue("2");
    m_builder->beginRepeatLoop();
    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endLoop();

    // Count returned by function
    m_builder->addConstValue(2);
    m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
    m_builder->beginRepeatLoop();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    // Nested
    m_builder->addConstValue(2);
    m_builder->beginRepeatLoop();
    {
        m_builder->addConstValue(2);
        m_builder->beginRepeatLoop();
        {
            m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
        }
        m_builder->endLoop();

        m_builder->addConstValue(2);
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(3);
        m_builder->beginRepeatLoop();
        {
            m_builder->addConstValue(3);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
        }
        m_builder->endLoop();
    }
    m_builder->endLoop();

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected =
        "no_args\n"
        "no_args\n"
        "no_args\n"
        "1_arg 0\n"
        "1_arg 0\n"
        "no_args\n"
        "no_args\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 2\n"
        "1_arg 3\n"
        "1_arg 3\n"
        "1_arg 3\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 2\n"
        "1_arg 3\n"
        "1_arg 3\n"
        "1_arg 3\n";

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, WhileLoop)
{
    // Const condition
    m_builder->beginLoopCondition();
    m_builder->addConstValue("false");
    m_builder->beginWhileLoop();
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    m_builder->beginLoopCondition();
    m_builder->addConstValue(false);
    m_builder->beginWhileLoop();
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    // Condition returned by function
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {});
    m_builder->beginLoopCondition();
    m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {});
    m_builder->addConstValue(2);
    m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number });
    m_builder->beginWhileLoop();
    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    // Nested
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {});
    m_builder->beginLoopCondition();
    m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {});
    m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number });
    m_builder->beginWhileLoop();
    {
        m_builder->beginLoopCondition();
        m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {});
        m_builder->addConstValue(3);
        m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number });
        m_builder->beginWhileLoop();
        {
            m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
            m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {});
        }
        m_builder->endLoop();

        m_builder->addConstValue(2);
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->beginLoopCondition();
        m_builder->addConstValue(false);
        m_builder->beginWhileLoop();
        {
            m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {});
        }
        m_builder->endLoop();
    }
    m_builder->endLoop();

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected =
        "1_arg 0\n"
        "1_arg 0\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 2\n";

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, RepeatUntilLoop)
{
    // Const condition
    m_builder->beginLoopCondition();
    m_builder->addConstValue("true");
    m_builder->beginRepeatUntilLoop();
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    m_builder->beginLoopCondition();
    m_builder->addConstValue(true);
    m_builder->beginRepeatUntilLoop();
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    // Condition returned by function
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {});
    m_builder->beginLoopCondition();
    m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {});
    m_builder->addConstValue(2);
    m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number });
    m_builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->beginRepeatUntilLoop();
    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    // Nested
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {});
    m_builder->beginLoopCondition();
    m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {});
    m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number });
    m_builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
    m_builder->beginRepeatUntilLoop();
    {
        m_builder->beginLoopCondition();
        m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {});
        m_builder->addConstValue(3);
        m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number });
        m_builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
        m_builder->beginRepeatUntilLoop();
        {
            m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
            m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {});
        }
        m_builder->endLoop();

        m_builder->addConstValue(2);
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->beginLoopCondition();
        m_builder->addConstValue(true);
        m_builder->beginRepeatUntilLoop();
        {
            m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {});
        }
        m_builder->endLoop();
    }
    m_builder->endLoop();

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&m_target);

    static const std::string expected =
        "1_arg 0\n"
        "1_arg 0\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 2\n";

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}
