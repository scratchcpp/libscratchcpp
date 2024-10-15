#include <scratchcpp/value.h>
#include <scratchcpp/dev/executablecode.h>
#include <dev/engine/internal/llvmcodebuilder.h>
#include <gmock/gmock.h>
#include <targetmock.h>

#include "testfunctions.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::Eq;

class LLVMCodeBuilderTest : public testing::Test
{
    public:
        void SetUp() override
        {
            test_function(nullptr, nullptr); // force dependency
        }

        void createBuilder(bool warp) { m_builder = std::make_unique<LLVMCodeBuilder>("test", warp); }

        std::unique_ptr<LLVMCodeBuilder> m_builder;
        TargetMock m_target; // NOTE: isStage() is used for call expectations
};

TEST_F(LLVMCodeBuilderTest, FunctionCalls)
{
    static const std::vector<bool> warpList = { false, true };

    for (bool warp : warpList) {
        createBuilder(warp);
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
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(LLVMCodeBuilderTest, ConstCasting)
{
    createBuilder(true);

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
    createBuilder(true);

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

TEST_F(LLVMCodeBuilderTest, Add)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createAdd();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->addConstValue(v2);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createAdd();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(expectedResult).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 25, 75);
    addOpTest(-500, 25, -475);
    addOpTest(-500, -25, -525);
    addOpTest("2.54", "6.28", 8.82);
    addOpTest(2.54, "-6.28", -3.74);
    addOpTest(true, true, 2);
    addOpTest("Infinity", "Infinity", std::numeric_limits<double>::infinity());
    addOpTest("Infinity", "-Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest("-Infinity", "Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest("-Infinity", "-Infinity", -std::numeric_limits<double>::infinity());
    addOpTest(1, "NaN", 1);
    addOpTest("NaN", 1, 1);
}

TEST_F(LLVMCodeBuilderTest, Subtract)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createSub();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->addConstValue(v2);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createSub();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(expectedResult).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 25, 25);
    addOpTest(-500, 25, -525);
    addOpTest(-500, -25, -475);
    addOpTest("2.54", "6.28", -3.74);
    addOpTest(2.54, "-6.28", 8.82);
    addOpTest(true, true, 0);
    addOpTest("Infinity", "Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest("Infinity", "-Infinity", std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", "Infinity", -std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", "-Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest(1, "NaN", 1);
    addOpTest("NaN", 1, -1);
}

TEST_F(LLVMCodeBuilderTest, Multiply)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createMul();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->addConstValue(v2);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createMul();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(expectedResult).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 2, 100);
    addOpTest(-500, 25, -12500);
    addOpTest("-500", -25, 12500);
    addOpTest("2.54", "6.28", 15.9512);
    addOpTest(true, true, 1);
    addOpTest("Infinity", "Infinity", std::numeric_limits<double>::infinity());
    addOpTest("Infinity", 0, std::numeric_limits<double>::quiet_NaN());
    addOpTest("Infinity", 2, std::numeric_limits<double>::infinity());
    addOpTest("Infinity", -2, -std::numeric_limits<double>::infinity());
    addOpTest("Infinity", "-Infinity", -std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", "Infinity", -std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", 0, std::numeric_limits<double>::quiet_NaN());
    addOpTest("-Infinity", 2, -std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", -2, std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", "-Infinity", std::numeric_limits<double>::infinity());
    addOpTest(1, "NaN", 0);
    addOpTest("NaN", 1, 0);
}

TEST_F(LLVMCodeBuilderTest, Divide)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createDiv();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->addConstValue(v2);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createDiv();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(expectedResult).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 2, 25);
    addOpTest(-500, 25, -20);
    addOpTest("-500", -25, 20);
    addOpTest("3.5", "2.5", 1.4);
    addOpTest(true, true, 1);
    addOpTest("Infinity", "Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest("Infinity", 0, std::numeric_limits<double>::infinity());
    addOpTest("Infinity", 2, std::numeric_limits<double>::infinity());
    addOpTest("Infinity", -2, -std::numeric_limits<double>::infinity());
    addOpTest("Infinity", "-Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest("-Infinity", "Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest("-Infinity", 0, -std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", 2, -std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", -2, std::numeric_limits<double>::infinity());
    addOpTest("-Infinity", "-Infinity", std::numeric_limits<double>::quiet_NaN());
    addOpTest(0, "Infinity", 0);
    addOpTest(2, "Infinity", 0);
    addOpTest(-2, "Infinity", 0);
    addOpTest(0, "-Infinity", 0);
    addOpTest(2, "-Infinity", 0);
    addOpTest(-2, "-Infinity", 0);
    addOpTest(1, "NaN", std::numeric_limits<double>::infinity());
    addOpTest("NaN", 1, 0);
    addOpTest(5, 0, std::numeric_limits<double>::infinity());
    addOpTest(-5, 0, -std::numeric_limits<double>::infinity());
    addOpTest(0, 0, std::numeric_limits<double>::quiet_NaN());
}

TEST_F(LLVMCodeBuilderTest, Yield)
{
    auto build = [this]() {
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
    };

    // Without warp
    createBuilder(false);
    build();

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

    // With warp
    createBuilder(true);
    build();
    code = m_builder->finalize();
    ctx = code->createExecutionContext(&m_target);

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

TEST_F(LLVMCodeBuilderTest, IfStatement)
{
    createBuilder(true);

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
    createBuilder(true);

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

    m_builder->addConstValue("2.4");
    m_builder->beginRepeatLoop();
    m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String });
    m_builder->endLoop();

    m_builder->addConstValue("2.5");
    m_builder->beginRepeatLoop();
    m_builder->addConstValue(1);
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
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 1\n"
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

    // Yield
    createBuilder(false);

    m_builder->addConstValue(3);
    m_builder->beginRepeatLoop();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    code = m_builder->finalize();
    ctx = code->createExecutionContext(&m_target);

    static const std::string expected1 = "no_args\n";

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));

    for (int i = 0; i < 3; i++) {
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_TRUE(testing::internal::GetCapturedStdout().empty());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // No warp no-op loop
    createBuilder(false);

    m_builder->addConstValue(0); // don't yield
    m_builder->beginRepeatLoop();
    m_builder->endLoop();

    code = m_builder->finalize();
    ctx = code->createExecutionContext(&m_target);
    code->run(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));
}

TEST_F(LLVMCodeBuilderTest, WhileLoop)
{
    createBuilder(true);

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

    // Yield
    createBuilder(false);

    m_builder->beginLoopCondition();
    m_builder->addConstValue(true);
    m_builder->beginWhileLoop();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    code = m_builder->finalize();
    ctx = code->createExecutionContext(&m_target);

    static const std::string expected1 = "no_args\n";

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));

    for (int i = 0; i < 10; i++) {
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }
}

TEST_F(LLVMCodeBuilderTest, RepeatUntilLoop)
{
    createBuilder(true);

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

    // Yield
    createBuilder(false);

    m_builder->beginLoopCondition();
    m_builder->addConstValue(false);
    m_builder->beginRepeatUntilLoop();
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {});
    m_builder->endLoop();

    code = m_builder->finalize();
    ctx = code->createExecutionContext(&m_target);

    static const std::string expected1 = "no_args\n";

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));

    for (int i = 0; i < 10; i++) {
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }
}
