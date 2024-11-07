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

        void callConstFuncForType(ValueType type)
        {
            switch (type) {
                case ValueType::Number:
                    m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
                    break;

                case ValueType::Bool:
                    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool });
                    break;

                case ValueType::String:
                    m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String });
                    break;

                default:
                    FAIL();
                    break;
            }
        }

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

    auto addOpTest = [this, &expected](Value v1, Value v2) {
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

        std::string str = (v1 + v2).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 25);
    addOpTest(-500, 25);
    addOpTest(-500, -25);
    addOpTest("2.54", "6.28");
    addOpTest(2.54, "-6.28");
    addOpTest(true, true);
    addOpTest("Infinity", "Infinity");
    addOpTest("Infinity", "-Infinity");
    addOpTest("-Infinity", "Infinity");
    addOpTest("-Infinity", "-Infinity");
    addOpTest(1, "NaN");
    addOpTest("NaN", 1);
}

TEST_F(LLVMCodeBuilderTest, Subtract)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2) {
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

        std::string str = (v1 - v2).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 25);
    addOpTest(-500, 25);
    addOpTest(-500, -25);
    addOpTest("2.54", "6.28");
    addOpTest(2.54, "-6.28");
    addOpTest(true, true);
    addOpTest("Infinity", "Infinity");
    addOpTest("Infinity", "-Infinity");
    addOpTest("-Infinity", "Infinity");
    addOpTest("-Infinity", "-Infinity");
    addOpTest(1, "NaN");
    addOpTest("NaN", 1);
}

TEST_F(LLVMCodeBuilderTest, Multiply)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2) {
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

        std::string str = (v1 * v2).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 2);
    addOpTest(-500, 25);
    addOpTest("-500", -25);
    addOpTest("2.54", "6.28");
    addOpTest(true, true);
    addOpTest("Infinity", "Infinity");
    addOpTest("Infinity", 0);
    addOpTest("Infinity", 2);
    addOpTest("Infinity", -2);
    addOpTest("Infinity", "-Infinity");
    addOpTest("-Infinity", "Infinity");
    addOpTest("-Infinity", 0);
    addOpTest("-Infinity", 2);
    addOpTest("-Infinity", -2);
    addOpTest("-Infinity", "-Infinity");
    addOpTest(1, "NaN");
    addOpTest("NaN", 1);
}

TEST_F(LLVMCodeBuilderTest, Divide)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2) {
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

        std::string str = (v1 / v2).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(50, 2);
    addOpTest(-500, 25);
    addOpTest("-500", -25);
    addOpTest("3.5", "2.5");
    addOpTest(true, true);
    addOpTest("Infinity", "Infinity");
    addOpTest("Infinity", 0);
    addOpTest("Infinity", 2);
    addOpTest("Infinity", -2);
    addOpTest("Infinity", "-Infinity");
    addOpTest("-Infinity", "Infinity");
    addOpTest("-Infinity", 0);
    addOpTest("-Infinity", 2);
    addOpTest("-Infinity", -2);
    addOpTest("-Infinity", "-Infinity");
    addOpTest(0, "Infinity");
    addOpTest(2, "Infinity");
    addOpTest(-2, "Infinity");
    addOpTest(0, "-Infinity");
    addOpTest(2, "-Infinity");
    addOpTest(-2, "-Infinity");
    addOpTest(1, "NaN");
    addOpTest("NaN", 1);
    addOpTest(5, 0);
    addOpTest(-5, 0);
    addOpTest(0, 0);
}

TEST_F(LLVMCodeBuilderTest, EqualComparison)
{
    auto addOpTest = [this](Value v1, Value v2) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createCmpEQ();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        callConstFuncForType(v1.type());
        m_builder->addConstValue(v2);
        callConstFuncForType(v2.type());
        m_builder->createCmpEQ();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(v1 == v2).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(10, 10);
    addOpTest(10, 8);
    addOpTest(8, 10);

    addOpTest(-4.25, -4.25);
    addOpTest(-4.25, 5.312);
    addOpTest(5.312, -4.25);

    addOpTest(true, true);
    addOpTest(true, false);
    addOpTest(false, true);

    addOpTest(1, true);
    addOpTest(1, false);

    addOpTest("abC def", "abC def");
    addOpTest("abC def", "abc dEf");
    addOpTest("abC def", "ghi Jkl");
    addOpTest("abC def", "hello world");

    addOpTest(" ", "");
    addOpTest(" ", "0");
    addOpTest(" ", 0);
    addOpTest(0, " ");
    addOpTest("", "0");
    addOpTest("", 0);
    addOpTest(0, "");
    addOpTest("0", 0);
    addOpTest(0, "0");

    addOpTest(5.25, "5.25");
    addOpTest("5.25", 5.25);
    addOpTest(5.25, " 5.25");
    addOpTest(" 5.25", 5.25);
    addOpTest(5.25, "5.25 ");
    addOpTest("5.25 ", 5.25);
    addOpTest(5.25, " 5.25 ");
    addOpTest(" 5.25 ", 5.25);
    addOpTest(5.25, "5.26");
    addOpTest("5.26", 5.25);
    addOpTest("5.25", "5.26");
    addOpTest(5, "5  ");
    addOpTest("5  ", 5);
    addOpTest(0, "1");
    addOpTest("1", 0);
    addOpTest(0, "test");
    addOpTest("test", 0);

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(inf, inf);
    addOpTest(-inf, -inf);
    addOpTest(nan, nan);
    addOpTest(inf, -inf);
    addOpTest(-inf, inf);
    addOpTest(inf, nan);
    addOpTest(nan, inf);
    addOpTest(-inf, nan);
    addOpTest(nan, -inf);

    addOpTest(5, inf);
    addOpTest(5, -inf);
    addOpTest(5, nan);
    addOpTest(0, nan);

    addOpTest(true, "true");
    addOpTest("true", true);
    addOpTest(false, "false");
    addOpTest("false", false);
    addOpTest(false, "true");
    addOpTest("true", false);
    addOpTest(true, "false");
    addOpTest("false", true);
    addOpTest(true, "TRUE");
    addOpTest("TRUE", true);
    addOpTest(false, "FALSE");
    addOpTest("FALSE", false);

    addOpTest(true, "00001");
    addOpTest("00001", true);
    addOpTest(true, "00000");
    addOpTest("00000", true);
    addOpTest(false, "00000");
    addOpTest("00000", false);

    addOpTest("true", 1);
    addOpTest(1, "true");
    addOpTest("true", 0);
    addOpTest(0, "true");
    addOpTest("false", 0);
    addOpTest(0, "false");
    addOpTest("false", 1);
    addOpTest(1, "false");

    addOpTest("true", "TRUE");
    addOpTest("true", "FALSE");
    addOpTest("false", "FALSE");
    addOpTest("false", "TRUE");

    addOpTest(true, inf);
    addOpTest(true, -inf);
    addOpTest(true, nan);
    addOpTest(false, inf);
    addOpTest(false, -inf);
    addOpTest(false, nan);

    addOpTest("Infinity", inf);
    addOpTest("Infinity", -inf);
    addOpTest("Infinity", nan);
    addOpTest("infinity", inf);
    addOpTest("infinity", -inf);
    addOpTest("infinity", nan);
    addOpTest("-Infinity", inf);
    addOpTest("-Infinity", -inf);
    addOpTest("-Infinity", nan);
    addOpTest("-infinity", inf);
    addOpTest("-infinity", -inf);
    addOpTest("-infinity", nan);
    addOpTest("NaN", inf);
    addOpTest("NaN", -inf);
    addOpTest("NaN", nan);
    addOpTest("nan", inf);
    addOpTest("nan", -inf);
    addOpTest("nan", nan);

    addOpTest(inf, "abc");
    addOpTest(inf, " ");
    addOpTest(inf, "");
    addOpTest(inf, "0");
    addOpTest(-inf, "abc");
    addOpTest(-inf, " ");
    addOpTest(-inf, "");
    addOpTest(-inf, "0");
    addOpTest(nan, "abc");
    addOpTest(nan, " ");
    addOpTest(nan, "");
    addOpTest(nan, "0");
}

TEST_F(LLVMCodeBuilderTest, GreaterAndLowerThanComparison)
{
    auto addOpTest = [this](Value v1, Value v2) {
        // GT
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createCmpGT();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        callConstFuncForType(v1.type());
        m_builder->addConstValue(v2);
        callConstFuncForType(v2.type());
        m_builder->createCmpGT();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(v1 > v2).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << "GT: " << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;

        // LT
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createCmpLT();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        callConstFuncForType(v1.type());
        m_builder->addConstValue(v2);
        callConstFuncForType(v2.type());
        m_builder->createCmpLT();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        str = Value(v1 < v2).toString() + '\n';
        expected = str + str;

        code = m_builder->finalize();
        ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << "LT: " << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(10, 10);
    addOpTest(10, 8);
    addOpTest(8, 10);

    addOpTest(-4.25, -4.25);
    addOpTest(-4.25, 5.312);
    addOpTest(5.312, -4.25);

    addOpTest(true, true);
    addOpTest(true, false);
    addOpTest(false, true);

    addOpTest(1, true);
    addOpTest(1, false);

    addOpTest("abC def", "abC def");
    addOpTest("abC def", "abc dEf");
    addOpTest("abC def", "ghi Jkl");
    addOpTest("ghi Jkl", "abC def");
    addOpTest("abC def", "hello world");

    addOpTest(" ", "");
    addOpTest(" ", "0");
    addOpTest(" ", 0);
    addOpTest(0, " ");
    addOpTest("", "0");
    addOpTest("", 0);
    addOpTest(0, "");
    addOpTest("0", 0);
    addOpTest(0, "0");

    addOpTest(5.25, "5.25");
    addOpTest("5.25", 5.25);
    addOpTest(5.25, " 5.25");
    addOpTest(" 5.25", 5.25);
    addOpTest(5.25, "5.25 ");
    addOpTest("5.25 ", 5.25);
    addOpTest(5.25, " 5.25 ");
    addOpTest(" 5.25 ", 5.25);
    addOpTest(5.25, "5.26");
    addOpTest("5.26", 5.25);
    addOpTest("5.25", "5.26");
    addOpTest(5, "5  ");
    addOpTest("5  ", 5);
    addOpTest(0, "1");
    addOpTest("1", 0);
    addOpTest(0, "test");
    addOpTest("test", 0);

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(inf, inf);
    addOpTest(-inf, -inf);
    addOpTest(nan, nan);
    addOpTest(inf, -inf);
    addOpTest(-inf, inf);
    addOpTest(inf, nan);
    addOpTest(nan, inf);
    addOpTest(-inf, nan);
    addOpTest(nan, -inf);

    addOpTest(5, inf);
    addOpTest(inf, 5);
    addOpTest(5, -inf);
    addOpTest(-inf, 5);
    addOpTest(5, nan);
    addOpTest(nan, 5);
    addOpTest(0, nan);
    addOpTest(nan, 0);

    addOpTest(true, "true");
    addOpTest("true", true);
    addOpTest(false, "false");
    addOpTest("false", false);
    addOpTest(false, "true");
    addOpTest("true", false);
    addOpTest(true, "false");
    addOpTest("false", true);
    addOpTest(true, "TRUE");
    addOpTest("TRUE", true);
    addOpTest(false, "FALSE");
    addOpTest("FALSE", false);

    addOpTest(true, "00001");
    addOpTest("00001", true);
    addOpTest(true, "00000");
    addOpTest("00000", true);
    addOpTest(false, "00000");
    addOpTest("00000", false);

    addOpTest("true", 1);
    addOpTest(1, "true");
    addOpTest("true", 0);
    addOpTest(0, "true");
    addOpTest("false", 0);
    addOpTest(0, "false");
    addOpTest("false", 1);
    addOpTest(1, "false");

    addOpTest("true", "TRUE");
    addOpTest("true", "FALSE");
    addOpTest("false", "FALSE");
    addOpTest("false", "TRUE");

    addOpTest(true, inf);
    addOpTest(inf, true);
    addOpTest(true, -inf);
    addOpTest(-inf, true);
    addOpTest(true, nan);
    addOpTest(nan, true);
    addOpTest(false, inf);
    addOpTest(inf, false);
    addOpTest(false, -inf);
    addOpTest(-inf, false);
    addOpTest(false, nan);
    addOpTest(nan, false);

    addOpTest("Infinity", inf);
    addOpTest("Infinity", -inf);
    addOpTest("Infinity", nan);
    addOpTest("infinity", inf);
    addOpTest("infinity", -inf);
    addOpTest("infinity", nan);
    addOpTest("-Infinity", inf);
    addOpTest("-Infinity", -inf);
    addOpTest("-Infinity", nan);
    addOpTest("-infinity", inf);
    addOpTest("-infinity", -inf);
    addOpTest("-infinity", nan);
    addOpTest("NaN", inf);
    addOpTest("NaN", -inf);
    addOpTest("NaN", nan);
    addOpTest("nan", inf);
    addOpTest("nan", -inf);
    addOpTest("nan", nan);

    addOpTest(inf, "abc");
    addOpTest(inf, " ");
    addOpTest(inf, "");
    addOpTest(inf, "0");
    addOpTest(-inf, "abc");
    addOpTest(-inf, " ");
    addOpTest(-inf, "");
    addOpTest(-inf, "0");
    addOpTest(nan, "abc");
    addOpTest(nan, " ");
    addOpTest(nan, "");
    addOpTest(nan, "0");
}

TEST_F(LLVMCodeBuilderTest, AndOr)
{
    auto addOpTest = [this](Value v1, Value v2) {
        // And
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createAnd();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        callConstFuncForType(v1.type());
        m_builder->addConstValue(v2);
        callConstFuncForType(v2.type());
        m_builder->createAnd();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(v1.toBool() && v2.toBool()).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << "AND: " << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;

        // Or
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createOr();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        callConstFuncForType(v1.type());
        m_builder->addConstValue(v2);
        callConstFuncForType(v2.type());
        m_builder->createOr();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        str = Value(v1.toBool() || v2.toBool()).toString() + '\n';
        expected = str + str;

        code = m_builder->finalize();
        ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << "OR: " << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(10, 8);
    addOpTest(-4.25, -4.25);
    addOpTest(-4.25, 5.312);

    addOpTest(true, true);
    addOpTest(true, false);
    addOpTest(false, true);

    addOpTest(1, true);
    addOpTest(1, false);

    addOpTest("abc", "def");
    addOpTest("true", "true");
    addOpTest("true", "false");
    addOpTest("false", "true");
    addOpTest("false", "false");

    addOpTest(5.25, "5.25");
    addOpTest("5.25", 5.25);
    addOpTest(0, "1");
    addOpTest("1", 0);
    addOpTest(0, "test");
    addOpTest("test", 0);

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(inf, inf);
    addOpTest(-inf, -inf);
    addOpTest(nan, nan);
    addOpTest(inf, -inf);
    addOpTest(-inf, inf);
    addOpTest(inf, nan);
    addOpTest(nan, inf);
    addOpTest(-inf, nan);
    addOpTest(nan, -inf);

    addOpTest(true, "true");
    addOpTest("true", true);
    addOpTest(false, "false");
    addOpTest("false", false);
    addOpTest(false, "true");
    addOpTest("true", false);
    addOpTest(true, "false");
    addOpTest("false", true);
    addOpTest(true, "TRUE");
    addOpTest("TRUE", true);
    addOpTest(false, "FALSE");
    addOpTest("FALSE", false);

    addOpTest(true, "00001");
    addOpTest("00001", true);
    addOpTest(true, "00000");
    addOpTest("00000", true);
    addOpTest(false, "00000");
    addOpTest("00000", false);

    addOpTest("true", 1);
    addOpTest(1, "true");
    addOpTest("true", 0);
    addOpTest(0, "true");
    addOpTest("false", 0);
    addOpTest(0, "false");
    addOpTest("false", 1);
    addOpTest(1, "false");

    addOpTest("true", "TRUE");
    addOpTest("true", "FALSE");
    addOpTest("false", "FALSE");
    addOpTest("false", "TRUE");

    addOpTest(true, inf);
    addOpTest(inf, true);
    addOpTest(true, -inf);
    addOpTest(-inf, true);
    addOpTest(true, nan);
    addOpTest(nan, true);
    addOpTest(false, inf);
    addOpTest(inf, false);
    addOpTest(false, -inf);
    addOpTest(-inf, false);
    addOpTest(false, nan);
    addOpTest(nan, false);

    addOpTest("Infinity", inf);
    addOpTest("Infinity", -inf);
    addOpTest("Infinity", nan);
    addOpTest("infinity", inf);
    addOpTest("infinity", -inf);
    addOpTest("infinity", nan);
    addOpTest("-Infinity", inf);
    addOpTest("-Infinity", -inf);
    addOpTest("-Infinity", nan);
    addOpTest("-infinity", inf);
    addOpTest("-infinity", -inf);
    addOpTest("-infinity", nan);
    addOpTest("NaN", inf);
    addOpTest("NaN", -inf);
    addOpTest("NaN", nan);
    addOpTest("nan", inf);
    addOpTest("nan", -inf);
    addOpTest("nan", nan);
}

TEST_F(LLVMCodeBuilderTest, Not)
{
    auto addOpTest = [this](Value v) {
        createBuilder(true);

        m_builder->addConstValue(v);
        m_builder->createNot();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v);
        callConstFuncForType(v.type());
        m_builder->createNot();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = Value(!v.toBool()).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes = v.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << "NOT: " << quotes << v.toString() << quotes;
    };

    addOpTest(10);
    addOpTest(-4.25);
    addOpTest(5.312);
    addOpTest(1);
    addOpTest(0);

    addOpTest(true);
    addOpTest(false);

    addOpTest("abc");
    addOpTest("5.25");
    addOpTest("0");

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(inf);
    addOpTest(-inf);
    addOpTest(nan);

    addOpTest("true");
    addOpTest("false");
    addOpTest("TRUE");
    addOpTest("FALSE");

    addOpTest("00001");
    addOpTest("00000");

    addOpTest("Infinity");
    addOpTest("infinity");
    addOpTest("-Infinity");
    addOpTest("-infinity");
    addOpTest("NaN");
    addOpTest("nan");
}

TEST_F(LLVMCodeBuilderTest, Mod)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, Value v2) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->addConstValue(v2);
        m_builder->createMod();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->addConstValue(v2);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createMod();
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String });

        std::string str = (v1 % v2).toString() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        const std::string quotes2 = v2.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
    };

    addOpTest(4, 3);
    addOpTest(3, 3);
    addOpTest(2, 3);
    addOpTest(1, 3);
    addOpTest(0, 3);
    addOpTest(-1, 3);
    addOpTest(-2, 3);
    addOpTest(-3, 3);
    addOpTest(-4, 3);
    addOpTest(4.75, 2);
    addOpTest(-4.75, 2);
    addOpTest(-4.75, -2);
    addOpTest(4.75, -2);
    addOpTest(5, 0);
    addOpTest(-5, 0);
    addOpTest(-2.5, "Infinity");
    addOpTest(-1.2, "-Infinity");
    addOpTest(2.5, "Infinity");
    addOpTest(1.2, "-Infinity");
    addOpTest("Infinity", 2);
    addOpTest("-Infinity", 2);
    addOpTest("Infinity", -2);
    addOpTest("-Infinity", -2);
    addOpTest(3, "NaN");
    addOpTest(-3, "NaN");
    addOpTest("NaN", 5);
    addOpTest("NaN", -5);
}

TEST_F(LLVMCodeBuilderTest, Round)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createRound();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createRound();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(4.0, 4.0);
    addOpTest(3.2, 3.0);
    addOpTest(3.5, 4.0);
    addOpTest(3.6, 4.0);
    addOpTest(-2.4, -2.0);
    addOpTest(-2.5, -2.0);
    addOpTest(-2.6, -3.0);
    addOpTest(-0.4, -0.0);
    addOpTest(-0.5, -0.0);
    addOpTest(-0.51, -1.0);
    addOpTest(inf, inf);
    addOpTest(-inf, -inf);
    addOpTest(nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Abs)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createAbs();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createAbs();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(4.0, 4.0);
    addOpTest(3.2, 3.2);
    addOpTest(-2.0, 2.0);
    addOpTest(-2.5, 2.5);
    addOpTest(-2.6, 2.6);
    addOpTest(0.0, 0.0);
    addOpTest(-0.0, 0.0);
    addOpTest(inf, inf);
    addOpTest(-inf, inf);
    addOpTest(nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Floor)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createFloor();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createFloor();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(4.0, 4.0);
    addOpTest(3.2, 3.0);
    addOpTest(3.5, 3.0);
    addOpTest(3.6, 3.0);
    addOpTest(0.0, 0.0);
    addOpTest(-0.0, -0.0);
    addOpTest(-2.4, -3.0);
    addOpTest(-2.5, -3.0);
    addOpTest(-2.6, -3.0);
    addOpTest(-0.4, -1.0);
    addOpTest(-0.5, -1.0);
    addOpTest(-0.51, -1.0);
    addOpTest(inf, inf);
    addOpTest(-inf, -inf);
    addOpTest(nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Ceil)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createCeil();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createCeil();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(8.0, 8.0);
    addOpTest(3.2, 4.0);
    addOpTest(3.5, 4.0);
    addOpTest(3.6, 4.0);
    addOpTest(0.4, 1.0);
    addOpTest(0.0, 0.0);
    addOpTest(-0.0, -0.0);
    addOpTest(-2.4, -2.0);
    addOpTest(-2.5, -2.0);
    addOpTest(-2.6, -2.0);
    addOpTest(-0.4, -0.0);
    addOpTest(-0.5, -0.0);
    addOpTest(-0.51, -0.0);
    addOpTest(inf, inf);
    addOpTest(-inf, -inf);
    addOpTest(nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Sqrt)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createSqrt();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createSqrt();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(16.0, 4.0);
    addOpTest(0.04, 0.2);
    addOpTest(0.0, 0.0);
    addOpTest(-0.0, 0.0);
    addOpTest(-4.0, -nan); // negative NaN shouldn't be a problem
    addOpTest(inf, inf);
    addOpTest(-inf, -nan);
    addOpTest(nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Sin)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createSin();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createSin();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(30.0, 0.5);
    addOpTest(90.0, 1.0);
    addOpTest(2.8e-9, 0.0);
    addOpTest(2.9e-9, 1e-10);
    addOpTest(570.0, -0.5);
    addOpTest(-30.0, -0.5);
    addOpTest(-90.0, -1.0);
    addOpTest(0.0, 0.0);
    addOpTest(-0.0, 0.0);
    addOpTest(inf, -nan); // negative NaN shouldn't be a problem
    addOpTest(-inf, -nan);
    addOpTest(nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Cos)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createCos();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createCos();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(60.0, 0.5);
    addOpTest(90.0, 0.0);
    addOpTest(600.0, -0.5);
    addOpTest(89.9999999971352, 1e-10);
    addOpTest(89.999999999, 0.0);
    addOpTest(-60.0, 0.5);
    addOpTest(-90.0, 0.0);
    addOpTest(0.0, 1.0);
    addOpTest(-0.0, 1.0);
    addOpTest(inf, -nan); // negative NaN shouldn't be a problem
    addOpTest(-inf, -nan);
    addOpTest(nan, 1.0);
}

TEST_F(LLVMCodeBuilderTest, Tan)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createTan();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createTan();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(45.0, 1.0);
    addOpTest(90.0, inf);
    addOpTest(270.0, -inf);
    addOpTest(450.0, inf);
    addOpTest(-90.0, -inf);
    addOpTest(-270.0, inf);
    addOpTest(-450.0, -inf);
    addOpTest(180.0, 0.0);
    addOpTest(-180.0, 0.0);
    addOpTest(2.87e-9, 1e-10);
    addOpTest(2.8647e-9, 0.0);
    addOpTest(0.0, 0.0);
    addOpTest(-0.0, 0.0);
    addOpTest(inf, -nan); // negative NaN shouldn't be a problem
    addOpTest(-inf, -nan);
    addOpTest(nan, 0.0);
}

TEST_F(LLVMCodeBuilderTest, Asin)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createAsin();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createAsin();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(1.0, 90.0);
    addOpTest(0.5, 30.0);
    addOpTest(0.0, 0.0);
    addOpTest(-0.0, 0.0);
    addOpTest(-0.5, -30.0);
    addOpTest(-1.0, -90.0);
    addOpTest(1.1, nan);
    addOpTest(-1.2, nan);
    addOpTest(inf, nan);
    addOpTest(-inf, nan);
    addOpTest(nan, 0.0);
}

TEST_F(LLVMCodeBuilderTest, Acos)
{
    std::string expected;

    auto addOpTest = [this, &expected](Value v1, double expectedResult) {
        createBuilder(true);

        m_builder->addConstValue(v1);
        m_builder->createAcos();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        m_builder->addConstValue(v1);
        m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number });
        m_builder->createAcos();
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number });

        std::stringstream stream;
        stream << expectedResult;
        std::string str = stream.str() + '\n';
        std::string expected = str + str;

        auto code = m_builder->finalize();
        auto ctx = code->createExecutionContext(&m_target);

        testing::internal::CaptureStdout();
        code->run(ctx.get());
        const std::string quotes1 = v1.isString() ? "\"" : "";
        ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1;
    };

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    addOpTest(1.0, 0.0);
    addOpTest(0.5, 60.0);
    addOpTest(0.0, 90.0);
    addOpTest(-0.0, 90.0);
    addOpTest(-0.5, 120.0);
    addOpTest(-1.0, 180.0);
    addOpTest(1.1, nan);
    addOpTest(-1.2, nan);
    addOpTest(inf, nan);
    addOpTest(-inf, nan);
    addOpTest(nan, 90.0);
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
