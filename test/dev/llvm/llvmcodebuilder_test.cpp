#include <scratchcpp/value.h>
#include <scratchcpp/dev/executablecode.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <dev/engine/internal/llvm/llvmcodebuilder.h>
#include <gmock/gmock.h>
#include <targetmock.h>
#include <enginemock.h>

#include "testfunctions.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::Eq;

class LLVMCodeBuilderTest : public testing::Test
{
    public:
        enum class OpType
        {
            Add,
            Sub,
            Mul,
            Div,
            CmpEQ,
            CmpGT,
            CmpLT,
            And,
            Or,
            Not,
            Mod,
            Round,
            Abs,
            Floor,
            Ceil,
            Sqrt,
            Sin,
            Cos,
            Tan,
            Asin,
            Acos,
            Atan,
            Ln,
            Log10,
            Exp,
            Exp10

        };

        void SetUp() override
        {
            test_function(nullptr, nullptr, nullptr, nullptr, nullptr); // force dependency
        }

        void createBuilder(Target *target, bool warp) { m_builder = std::make_unique<LLVMCodeBuilder>(target, "test", warp); }
        void createBuilder(bool warp) { createBuilder(nullptr, warp); }

        CompilerValue *callConstFuncForType(ValueType type, CompilerValue *arg)
        {
            switch (type) {
                case ValueType::Number:
                    return m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { arg });

                case ValueType::Bool:
                    return m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { arg });

                case ValueType::String:
                    return m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { arg });

                default:
                    EXPECT_TRUE(false);
                    return nullptr;
            }
        }

        CompilerValue *addOp(OpType type, CompilerValue *arg1, CompilerValue *arg2)
        {
            switch (type) {
                case OpType::Add:
                    return m_builder->createAdd(arg1, arg2);

                case OpType::Sub:
                    return m_builder->createSub(arg1, arg2);

                case OpType::Mul:
                    return m_builder->createMul(arg1, arg2);

                case OpType::Div:
                    return m_builder->createDiv(arg1, arg2);

                case OpType::CmpEQ:
                    return m_builder->createCmpEQ(arg1, arg2);

                case OpType::CmpGT:
                    return m_builder->createCmpGT(arg1, arg2);

                case OpType::CmpLT:
                    return m_builder->createCmpLT(arg1, arg2);

                case OpType::And:
                    return m_builder->createAnd(arg1, arg2);

                case OpType::Or:
                    return m_builder->createOr(arg1, arg2);

                case OpType::Mod:
                    return m_builder->createMod(arg1, arg2);

                default:
                    EXPECT_TRUE(false);
                    return nullptr;
            }
        }

        CompilerValue *addOp(OpType type, CompilerValue *arg)
        {
            switch (type) {
                case OpType::Not:
                    return m_builder->createNot(arg);

                case OpType::Round:
                    return m_builder->createRound(arg);

                case OpType::Abs:
                    return m_builder->createAbs(arg);

                case OpType::Floor:
                    return m_builder->createFloor(arg);

                case OpType::Ceil:
                    return m_builder->createCeil(arg);

                case OpType::Sqrt:
                    return m_builder->createSqrt(arg);

                case OpType::Sin:
                    return m_builder->createSin(arg);

                case OpType::Cos:
                    return m_builder->createCos(arg);

                case OpType::Tan:
                    return m_builder->createTan(arg);

                case OpType::Asin:
                    return m_builder->createAsin(arg);

                case OpType::Acos:
                    return m_builder->createAcos(arg);

                case OpType::Atan:
                    return m_builder->createAtan(arg);

                case OpType::Ln:
                    return m_builder->createLn(arg);

                case OpType::Log10:
                    return m_builder->createLog10(arg);

                case OpType::Exp:
                    return m_builder->createExp(arg);

                case OpType::Exp10:
                    return m_builder->createExp10(arg);

                default:
                    EXPECT_TRUE(false);
                    return nullptr;
            }
        }

        Value doOp(OpType type, const Value &v1, const Value &v2)
        {
            switch (type) {
                case OpType::Add:
                    return v1 + v2;

                case OpType::Sub:
                    return v1 - v2;

                case OpType::Mul:
                    return v1 * v2;

                case OpType::Div:
                    return v1 / v2;

                case OpType::CmpEQ:
                    return v1 == v2;

                case OpType::CmpGT:
                    return v1 > v2;

                case OpType::CmpLT:
                    return v1 < v2;

                case OpType::And:
                    return v1.toBool() && v2.toBool();

                case OpType::Or:
                    return v1.toBool() || v2.toBool();

                case OpType::Mod:
                    return v1 % v2;

                default:
                    EXPECT_TRUE(false);
                    return Value();
            }
        }

        Value doOp(OpType type, const Value &v)
        {
            switch (type) {
                case OpType::Not:
                    return !v.toBool();

                default:
                    EXPECT_TRUE(false);
                    return Value();
            }
        }

        void runOpTest(OpType type, const Value &v1, const Value &v2)
        {
            createBuilder(true);

            CompilerValue *arg1 = m_builder->addConstValue(v1);
            CompilerValue *arg2 = m_builder->addConstValue(v2);
            CompilerValue *ret = addOp(type, arg1, arg2);
            m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { ret });

            arg1 = m_builder->addConstValue(v1);
            arg1 = callConstFuncForType(v1.type(), arg1);
            arg2 = m_builder->addConstValue(v2);
            arg2 = callConstFuncForType(v2.type(), arg2);
            ret = addOp(type, arg1, arg2);
            m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { ret });

            std::string str = doOp(type, v1, v2).toString() + '\n';
            std::string expected = str + str;

            auto code = m_builder->finalize();
            auto ctx = code->createExecutionContext(&m_target);

            testing::internal::CaptureStdout();
            code->run(ctx.get());
            const std::string quotes1 = v1.isString() ? "\"" : "";
            const std::string quotes2 = v2.isString() ? "\"" : "";
            ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes1 << v1.toString() << quotes1 << " " << quotes2 << v2.toString() << quotes2;
        };

        void runOpTest(OpType type, const Value &v)
        {
            createBuilder(true);

            CompilerValue *arg = m_builder->addConstValue(v);
            CompilerValue *ret = addOp(type, arg);
            m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { ret });

            arg = m_builder->addConstValue(v);
            arg = callConstFuncForType(v.type(), arg);
            ret = addOp(type, arg);
            m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { ret });

            std::string str = doOp(type, v).toString() + '\n';
            std::string expected = str + str;

            auto code = m_builder->finalize();
            auto ctx = code->createExecutionContext(&m_target);

            testing::internal::CaptureStdout();
            code->run(ctx.get());
            const std::string quotes = v.isString() ? "\"" : "";
            ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << "NOT: " << quotes << v.toString() << quotes;
        };

        void runUnaryNumOpTest(OpType type, const Value &v, double expectedResult)
        {
            createBuilder(true);

            CompilerValue *arg = m_builder->addConstValue(v);
            CompilerValue *ret = addOp(type, arg);
            m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { ret });

            arg = m_builder->addConstValue(v);
            arg = callConstFuncForType(v.type(), arg);
            ret = addOp(type, arg);
            m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { ret });

            std::stringstream stream;
            stream << expectedResult;
            std::string str = stream.str() + '\n';
            std::string expected = str + str;

            auto code = m_builder->finalize();
            auto ctx = code->createExecutionContext(&m_target);

            testing::internal::CaptureStdout();
            code->run(ctx.get());
            const std::string quotes = v.isString() ? "\"" : "";
            ASSERT_THAT(testing::internal::GetCapturedStdout(), Eq(expected)) << quotes << v.toString() << quotes;
        };

        std::unique_ptr<LLVMCodeBuilder> m_builder;
        TargetMock m_target; // NOTE: isStage() is used for call expectations
};

TEST_F(LLVMCodeBuilderTest, FunctionCalls)
{
    static const std::vector<bool> warpList = { false, true };

    for (bool warp : warpList) {
        createBuilder(warp);
        m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

        CompilerValue *v = m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addConstValue("1");
        v = m_builder->addFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        CompilerValue *v1 = m_builder->addConstValue("2");
        CompilerValue *v2 = m_builder->addConstValue("3");
        m_builder->addFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String }, { v, v1, v2 });

        v = m_builder->addConstValue("test");
        v1 = m_builder->addConstValue("4");
        v2 = m_builder->addConstValue("5");
        v = m_builder->addFunctionCall(
            "test_function_3_args_ret",
            Compiler::StaticType::String,
            { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String },
            { v, v1, v2 });
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
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

    CompilerValue *v = m_builder->addConstValue(5.2);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });
    v = m_builder->addConstValue("-24.156");
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addConstValue(true);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue("false");
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue("123");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("hello world");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

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
    CompilerValue *v = m_builder->addConstValue(5.2);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Number -> bool
    v = m_builder->addConstValue(-24.156);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue(0);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // Number -> string
    v = m_builder->addConstValue(59.8);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Bool -> number
    v = m_builder->addConstValue(true);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addConstValue(false);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Bool -> bool
    v = m_builder->addConstValue(true);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue(false);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // Bool -> string
    v = m_builder->addConstValue(true);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(false);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // String -> number
    v = m_builder->addConstValue("5.2");
    v = callConstFuncForType(ValueType::String, v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // String -> bool
    v = m_builder->addConstValue("abc");
    v = callConstFuncForType(ValueType::String, v);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue("false");
    v = callConstFuncForType(ValueType::String, v);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // String -> string
    v = m_builder->addConstValue("hello world");
    v = callConstFuncForType(ValueType::String, v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

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
    runOpTest(OpType::Add, 50, 25);
    runOpTest(OpType::Add, -500, 25);
    runOpTest(OpType::Add, -500, -25);
    runOpTest(OpType::Add, "2.54", "6.28");
    runOpTest(OpType::Add, 2.54, "-6.28");
    runOpTest(OpType::Add, true, true);
    runOpTest(OpType::Add, "Infinity", "Infinity");
    runOpTest(OpType::Add, "Infinity", "-Infinity");
    runOpTest(OpType::Add, "-Infinity", "Infinity");
    runOpTest(OpType::Add, "-Infinity", "-Infinity");
    runOpTest(OpType::Add, 1, "NaN");
    runOpTest(OpType::Add, "NaN", 1);
}

TEST_F(LLVMCodeBuilderTest, Subtract)
{
    runOpTest(OpType::Sub, 50, 25);
    runOpTest(OpType::Sub, -500, 25);
    runOpTest(OpType::Sub, -500, -25);
    runOpTest(OpType::Sub, "2.54", "6.28");
    runOpTest(OpType::Sub, 2.54, "-6.28");
    runOpTest(OpType::Sub, true, true);
    runOpTest(OpType::Sub, "Infinity", "Infinity");
    runOpTest(OpType::Sub, "Infinity", "-Infinity");
    runOpTest(OpType::Sub, "-Infinity", "Infinity");
    runOpTest(OpType::Sub, "-Infinity", "-Infinity");
    runOpTest(OpType::Sub, 1, "NaN");
    runOpTest(OpType::Sub, "NaN", 1);
}

TEST_F(LLVMCodeBuilderTest, Multiply)
{
    runOpTest(OpType::Mul, 50, 2);
    runOpTest(OpType::Mul, -500, 25);
    runOpTest(OpType::Mul, "-500", -25);
    runOpTest(OpType::Mul, "2.54", "6.28");
    runOpTest(OpType::Mul, true, true);
    runOpTest(OpType::Mul, "Infinity", "Infinity");
    runOpTest(OpType::Mul, "Infinity", 0);
    runOpTest(OpType::Mul, "Infinity", 2);
    runOpTest(OpType::Mul, "Infinity", -2);
    runOpTest(OpType::Mul, "Infinity", "-Infinity");
    runOpTest(OpType::Mul, "-Infinity", "Infinity");
    runOpTest(OpType::Mul, "-Infinity", 0);
    runOpTest(OpType::Mul, "-Infinity", 2);
    runOpTest(OpType::Mul, "-Infinity", -2);
    runOpTest(OpType::Mul, "-Infinity", "-Infinity");
    runOpTest(OpType::Mul, 1, "NaN");
    runOpTest(OpType::Mul, "NaN", 1);
}

TEST_F(LLVMCodeBuilderTest, Divide)
{
    runOpTest(OpType::Div, 50, 2);
    runOpTest(OpType::Div, -500, 25);
    runOpTest(OpType::Div, "-500", -25);
    runOpTest(OpType::Div, "3.5", "2.5");
    runOpTest(OpType::Div, true, true);
    runOpTest(OpType::Div, "Infinity", "Infinity");
    runOpTest(OpType::Div, "Infinity", 0);
    runOpTest(OpType::Div, "Infinity", 2);
    runOpTest(OpType::Div, "Infinity", -2);
    runOpTest(OpType::Div, "Infinity", "-Infinity");
    runOpTest(OpType::Div, "-Infinity", "Infinity");
    runOpTest(OpType::Div, "-Infinity", 0);
    runOpTest(OpType::Div, "-Infinity", 2);
    runOpTest(OpType::Div, "-Infinity", -2);
    runOpTest(OpType::Div, "-Infinity", "-Infinity");
    runOpTest(OpType::Div, 0, "Infinity");
    runOpTest(OpType::Div, 2, "Infinity");
    runOpTest(OpType::Div, -2, "Infinity");
    runOpTest(OpType::Div, 0, "-Infinity");
    runOpTest(OpType::Div, 2, "-Infinity");
    runOpTest(OpType::Div, -2, "-Infinity");
    runOpTest(OpType::Div, 1, "NaN");
    runOpTest(OpType::Div, "NaN", 1);
    runOpTest(OpType::Div, 5, 0);
    runOpTest(OpType::Div, -5, 0);
    runOpTest(OpType::Div, 0, 0);
}

TEST_F(LLVMCodeBuilderTest, EqualComparison)
{
    runOpTest(OpType::CmpEQ, 10, 10);
    runOpTest(OpType::CmpEQ, 10, 8);
    runOpTest(OpType::CmpEQ, 8, 10);

    runOpTest(OpType::CmpEQ, -4.25, -4.25);
    runOpTest(OpType::CmpEQ, -4.25, 5.312);
    runOpTest(OpType::CmpEQ, 5.312, -4.25);

    runOpTest(OpType::CmpEQ, true, true);
    runOpTest(OpType::CmpEQ, true, false);
    runOpTest(OpType::CmpEQ, false, true);

    runOpTest(OpType::CmpEQ, 1, true);
    runOpTest(OpType::CmpEQ, 1, false);

    runOpTest(OpType::CmpEQ, "abC def", "abC def");
    runOpTest(OpType::CmpEQ, "abC def", "abc dEf");
    runOpTest(OpType::CmpEQ, "abC def", "ghi Jkl");
    runOpTest(OpType::CmpEQ, "abC def", "hello world");

    runOpTest(OpType::CmpEQ, " ", "");
    runOpTest(OpType::CmpEQ, " ", "0");
    runOpTest(OpType::CmpEQ, " ", 0);
    runOpTest(OpType::CmpEQ, 0, " ");
    runOpTest(OpType::CmpEQ, "", "0");
    runOpTest(OpType::CmpEQ, "", 0);
    runOpTest(OpType::CmpEQ, 0, "");
    runOpTest(OpType::CmpEQ, "0", 0);
    runOpTest(OpType::CmpEQ, 0, "0");

    runOpTest(OpType::CmpEQ, 5.25, "5.25");
    runOpTest(OpType::CmpEQ, "5.25", 5.25);
    runOpTest(OpType::CmpEQ, 5.25, " 5.25");
    runOpTest(OpType::CmpEQ, " 5.25", 5.25);
    runOpTest(OpType::CmpEQ, 5.25, "5.25 ");
    runOpTest(OpType::CmpEQ, "5.25 ", 5.25);
    runOpTest(OpType::CmpEQ, 5.25, " 5.25 ");
    runOpTest(OpType::CmpEQ, " 5.25 ", 5.25);
    runOpTest(OpType::CmpEQ, 5.25, "5.26");
    runOpTest(OpType::CmpEQ, "5.26", 5.25);
    runOpTest(OpType::CmpEQ, "5.25", "5.26");
    runOpTest(OpType::CmpEQ, 5, "5  ");
    runOpTest(OpType::CmpEQ, "5  ", 5);
    runOpTest(OpType::CmpEQ, 0, "1");
    runOpTest(OpType::CmpEQ, "1", 0);
    runOpTest(OpType::CmpEQ, 0, "test");
    runOpTest(OpType::CmpEQ, "test", 0);

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runOpTest(OpType::CmpEQ, inf, inf);
    runOpTest(OpType::CmpEQ, -inf, -inf);
    runOpTest(OpType::CmpEQ, nan, nan);
    runOpTest(OpType::CmpEQ, inf, -inf);
    runOpTest(OpType::CmpEQ, -inf, inf);
    runOpTest(OpType::CmpEQ, inf, nan);
    runOpTest(OpType::CmpEQ, nan, inf);
    runOpTest(OpType::CmpEQ, -inf, nan);
    runOpTest(OpType::CmpEQ, nan, -inf);

    runOpTest(OpType::CmpEQ, 5, inf);
    runOpTest(OpType::CmpEQ, 5, -inf);
    runOpTest(OpType::CmpEQ, 5, nan);
    runOpTest(OpType::CmpEQ, 0, nan);

    runOpTest(OpType::CmpEQ, true, "true");
    runOpTest(OpType::CmpEQ, "true", true);
    runOpTest(OpType::CmpEQ, false, "false");
    runOpTest(OpType::CmpEQ, "false", false);
    runOpTest(OpType::CmpEQ, false, "true");
    runOpTest(OpType::CmpEQ, "true", false);
    runOpTest(OpType::CmpEQ, true, "false");
    runOpTest(OpType::CmpEQ, "false", true);
    runOpTest(OpType::CmpEQ, true, "TRUE");
    runOpTest(OpType::CmpEQ, "TRUE", true);
    runOpTest(OpType::CmpEQ, false, "FALSE");
    runOpTest(OpType::CmpEQ, "FALSE", false);

    runOpTest(OpType::CmpEQ, true, "00001");
    runOpTest(OpType::CmpEQ, "00001", true);
    runOpTest(OpType::CmpEQ, true, "00000");
    runOpTest(OpType::CmpEQ, "00000", true);
    runOpTest(OpType::CmpEQ, false, "00000");
    runOpTest(OpType::CmpEQ, "00000", false);

    runOpTest(OpType::CmpEQ, "true", 1);
    runOpTest(OpType::CmpEQ, 1, "true");
    runOpTest(OpType::CmpEQ, "true", 0);
    runOpTest(OpType::CmpEQ, 0, "true");
    runOpTest(OpType::CmpEQ, "false", 0);
    runOpTest(OpType::CmpEQ, 0, "false");
    runOpTest(OpType::CmpEQ, "false", 1);
    runOpTest(OpType::CmpEQ, 1, "false");

    runOpTest(OpType::CmpEQ, "true", "TRUE");
    runOpTest(OpType::CmpEQ, "true", "FALSE");
    runOpTest(OpType::CmpEQ, "false", "FALSE");
    runOpTest(OpType::CmpEQ, "false", "TRUE");

    runOpTest(OpType::CmpEQ, true, inf);
    runOpTest(OpType::CmpEQ, true, -inf);
    runOpTest(OpType::CmpEQ, true, nan);
    runOpTest(OpType::CmpEQ, false, inf);
    runOpTest(OpType::CmpEQ, false, -inf);
    runOpTest(OpType::CmpEQ, false, nan);

    runOpTest(OpType::CmpEQ, "Infinity", inf);
    runOpTest(OpType::CmpEQ, "Infinity", -inf);
    runOpTest(OpType::CmpEQ, "Infinity", nan);
    runOpTest(OpType::CmpEQ, "infinity", inf);
    runOpTest(OpType::CmpEQ, "infinity", -inf);
    runOpTest(OpType::CmpEQ, "infinity", nan);
    runOpTest(OpType::CmpEQ, "-Infinity", inf);
    runOpTest(OpType::CmpEQ, "-Infinity", -inf);
    runOpTest(OpType::CmpEQ, "-Infinity", nan);
    runOpTest(OpType::CmpEQ, "-infinity", inf);
    runOpTest(OpType::CmpEQ, "-infinity", -inf);
    runOpTest(OpType::CmpEQ, "-infinity", nan);
    runOpTest(OpType::CmpEQ, "NaN", inf);
    runOpTest(OpType::CmpEQ, "NaN", -inf);
    runOpTest(OpType::CmpEQ, "NaN", nan);
    runOpTest(OpType::CmpEQ, "nan", inf);
    runOpTest(OpType::CmpEQ, "nan", -inf);
    runOpTest(OpType::CmpEQ, "nan", nan);

    runOpTest(OpType::CmpEQ, inf, "abc");
    runOpTest(OpType::CmpEQ, inf, " ");
    runOpTest(OpType::CmpEQ, inf, "");
    runOpTest(OpType::CmpEQ, inf, "0");
    runOpTest(OpType::CmpEQ, -inf, "abc");
    runOpTest(OpType::CmpEQ, -inf, " ");
    runOpTest(OpType::CmpEQ, -inf, "");
    runOpTest(OpType::CmpEQ, -inf, "0");
    runOpTest(OpType::CmpEQ, nan, "abc");
    runOpTest(OpType::CmpEQ, nan, " ");
    runOpTest(OpType::CmpEQ, nan, "");
    runOpTest(OpType::CmpEQ, nan, "0");
}

TEST_F(LLVMCodeBuilderTest, GreaterAndLowerThanComparison)
{
    std::vector<OpType> opTypes = { OpType::CmpGT, OpType::CmpLT };

    for (OpType type : opTypes) {
        runOpTest(type, 10, 10);
        runOpTest(type, 10, 8);
        runOpTest(type, 8, 10);

        runOpTest(type, -4.25, -4.25);
        runOpTest(type, -4.25, 5.312);
        runOpTest(type, 5.312, -4.25);

        runOpTest(type, true, true);
        runOpTest(type, true, false);
        runOpTest(type, false, true);

        runOpTest(type, 1, true);
        runOpTest(type, 1, false);

        runOpTest(type, "abC def", "abC def");
        runOpTest(type, "abC def", "abc dEf");
        runOpTest(type, "abC def", "ghi Jkl");
        runOpTest(type, "ghi Jkl", "abC def");
        runOpTest(type, "abC def", "hello world");

        runOpTest(type, " ", "");
        runOpTest(type, " ", "0");
        runOpTest(type, " ", 0);
        runOpTest(type, 0, " ");
        runOpTest(type, "", "0");
        runOpTest(type, "", 0);
        runOpTest(type, 0, "");
        runOpTest(type, "0", 0);
        runOpTest(type, 0, "0");

        runOpTest(type, 5.25, "5.25");
        runOpTest(type, "5.25", 5.25);
        runOpTest(type, 5.25, " 5.25");
        runOpTest(type, " 5.25", 5.25);
        runOpTest(type, 5.25, "5.25 ");
        runOpTest(type, "5.25 ", 5.25);
        runOpTest(type, 5.25, " 5.25 ");
        runOpTest(type, " 5.25 ", 5.25);
        runOpTest(type, 5.25, "5.26");
        runOpTest(type, "5.26", 5.25);
        runOpTest(type, "5.25", "5.26");
        runOpTest(type, 5, "5  ");
        runOpTest(type, "5  ", 5);
        runOpTest(type, 0, "1");
        runOpTest(type, "1", 0);
        runOpTest(type, 0, "test");
        runOpTest(type, "test", 0);

        static const double inf = std::numeric_limits<double>::infinity();
        static const double nan = std::numeric_limits<double>::quiet_NaN();

        runOpTest(type, inf, inf);
        runOpTest(type, -inf, -inf);
        runOpTest(type, nan, nan);
        runOpTest(type, inf, -inf);
        runOpTest(type, -inf, inf);
        runOpTest(type, inf, nan);
        runOpTest(type, nan, inf);
        runOpTest(type, -inf, nan);
        runOpTest(type, nan, -inf);

        runOpTest(type, 5, inf);
        runOpTest(type, inf, 5);
        runOpTest(type, 5, -inf);
        runOpTest(type, -inf, 5);
        runOpTest(type, 5, nan);
        runOpTest(type, nan, 5);
        runOpTest(type, 0, nan);
        runOpTest(type, nan, 0);

        runOpTest(type, true, "true");
        runOpTest(type, "true", true);
        runOpTest(type, false, "false");
        runOpTest(type, "false", false);
        runOpTest(type, false, "true");
        runOpTest(type, "true", false);
        runOpTest(type, true, "false");
        runOpTest(type, "false", true);
        runOpTest(type, true, "TRUE");
        runOpTest(type, "TRUE", true);
        runOpTest(type, false, "FALSE");
        runOpTest(type, "FALSE", false);

        runOpTest(type, true, "00001");
        runOpTest(type, "00001", true);
        runOpTest(type, true, "00000");
        runOpTest(type, "00000", true);
        runOpTest(type, false, "00000");
        runOpTest(type, "00000", false);

        runOpTest(type, "true", 1);
        runOpTest(type, 1, "true");
        runOpTest(type, "true", 0);
        runOpTest(type, 0, "true");
        runOpTest(type, "false", 0);
        runOpTest(type, 0, "false");
        runOpTest(type, "false", 1);
        runOpTest(type, 1, "false");

        runOpTest(type, "true", "TRUE");
        runOpTest(type, "true", "FALSE");
        runOpTest(type, "false", "FALSE");
        runOpTest(type, "false", "TRUE");

        runOpTest(type, true, inf);
        runOpTest(type, inf, true);
        runOpTest(type, true, -inf);
        runOpTest(type, -inf, true);
        runOpTest(type, true, nan);
        runOpTest(type, nan, true);
        runOpTest(type, false, inf);
        runOpTest(type, inf, false);
        runOpTest(type, false, -inf);
        runOpTest(type, -inf, false);
        runOpTest(type, false, nan);
        runOpTest(type, nan, false);

        runOpTest(type, "Infinity", inf);
        runOpTest(type, "Infinity", -inf);
        runOpTest(type, "Infinity", nan);
        runOpTest(type, "infinity", inf);
        runOpTest(type, "infinity", -inf);
        runOpTest(type, "infinity", nan);
        runOpTest(type, "-Infinity", inf);
        runOpTest(type, "-Infinity", -inf);
        runOpTest(type, "-Infinity", nan);
        runOpTest(type, "-infinity", inf);
        runOpTest(type, "-infinity", -inf);
        runOpTest(type, "-infinity", nan);
        runOpTest(type, "NaN", inf);
        runOpTest(type, "NaN", -inf);
        runOpTest(type, "NaN", nan);
        runOpTest(type, "nan", inf);
        runOpTest(type, "nan", -inf);
        runOpTest(type, "nan", nan);

        runOpTest(type, inf, "abc");
        runOpTest(type, inf, " ");
        runOpTest(type, inf, "");
        runOpTest(type, inf, "0");
        runOpTest(type, -inf, "abc");
        runOpTest(type, -inf, " ");
        runOpTest(type, -inf, "");
        runOpTest(type, -inf, "0");
        runOpTest(type, nan, "abc");
        runOpTest(type, nan, " ");
        runOpTest(type, nan, "");
        runOpTest(type, nan, "0");
    }
}

TEST_F(LLVMCodeBuilderTest, AndOr)
{
    std::vector<OpType> opTypes = { OpType::And, OpType::Or };

    for (OpType type : opTypes) {
        runOpTest(type, 10, 8);
        runOpTest(type, -4.25, -4.25);
        runOpTest(type, -4.25, 5.312);

        runOpTest(type, true, true);
        runOpTest(type, true, false);
        runOpTest(type, false, true);

        runOpTest(type, 1, true);
        runOpTest(type, 1, false);

        runOpTest(type, "abc", "def");
        runOpTest(type, "true", "true");
        runOpTest(type, "true", "false");
        runOpTest(type, "false", "true");
        runOpTest(type, "false", "false");

        runOpTest(type, 5.25, "5.25");
        runOpTest(type, "5.25", 5.25);
        runOpTest(type, 0, "1");
        runOpTest(type, "1", 0);
        runOpTest(type, 0, "test");
        runOpTest(type, "test", 0);

        static const double inf = std::numeric_limits<double>::infinity();
        static const double nan = std::numeric_limits<double>::quiet_NaN();

        runOpTest(type, inf, inf);
        runOpTest(type, -inf, -inf);
        runOpTest(type, nan, nan);
        runOpTest(type, inf, -inf);
        runOpTest(type, -inf, inf);
        runOpTest(type, inf, nan);
        runOpTest(type, nan, inf);
        runOpTest(type, -inf, nan);
        runOpTest(type, nan, -inf);

        runOpTest(type, true, "true");
        runOpTest(type, "true", true);
        runOpTest(type, false, "false");
        runOpTest(type, "false", false);
        runOpTest(type, false, "true");
        runOpTest(type, "true", false);
        runOpTest(type, true, "false");
        runOpTest(type, "false", true);
        runOpTest(type, true, "TRUE");
        runOpTest(type, "TRUE", true);
        runOpTest(type, false, "FALSE");
        runOpTest(type, "FALSE", false);

        runOpTest(type, true, "00001");
        runOpTest(type, "00001", true);
        runOpTest(type, true, "00000");
        runOpTest(type, "00000", true);
        runOpTest(type, false, "00000");
        runOpTest(type, "00000", false);

        runOpTest(type, "true", 1);
        runOpTest(type, 1, "true");
        runOpTest(type, "true", 0);
        runOpTest(type, 0, "true");
        runOpTest(type, "false", 0);
        runOpTest(type, 0, "false");
        runOpTest(type, "false", 1);
        runOpTest(type, 1, "false");

        runOpTest(type, "true", "TRUE");
        runOpTest(type, "true", "FALSE");
        runOpTest(type, "false", "FALSE");
        runOpTest(type, "false", "TRUE");

        runOpTest(type, true, inf);
        runOpTest(type, inf, true);
        runOpTest(type, true, -inf);
        runOpTest(type, -inf, true);
        runOpTest(type, true, nan);
        runOpTest(type, nan, true);
        runOpTest(type, false, inf);
        runOpTest(type, inf, false);
        runOpTest(type, false, -inf);
        runOpTest(type, -inf, false);
        runOpTest(type, false, nan);
        runOpTest(type, nan, false);

        runOpTest(type, "Infinity", inf);
        runOpTest(type, "Infinity", -inf);
        runOpTest(type, "Infinity", nan);
        runOpTest(type, "infinity", inf);
        runOpTest(type, "infinity", -inf);
        runOpTest(type, "infinity", nan);
        runOpTest(type, "-Infinity", inf);
        runOpTest(type, "-Infinity", -inf);
        runOpTest(type, "-Infinity", nan);
        runOpTest(type, "-infinity", inf);
        runOpTest(type, "-infinity", -inf);
        runOpTest(type, "-infinity", nan);
        runOpTest(type, "NaN", inf);
        runOpTest(type, "NaN", -inf);
        runOpTest(type, "NaN", nan);
        runOpTest(type, "nan", inf);
        runOpTest(type, "nan", -inf);
        runOpTest(type, "nan", nan);
    }
}

TEST_F(LLVMCodeBuilderTest, Not)
{
    runOpTest(OpType::Not, 10);
    runOpTest(OpType::Not, -4.25);
    runOpTest(OpType::Not, 5.312);
    runOpTest(OpType::Not, 1);
    runOpTest(OpType::Not, 0);

    runOpTest(OpType::Not, true);
    runOpTest(OpType::Not, false);

    runOpTest(OpType::Not, "abc");
    runOpTest(OpType::Not, "5.25");
    runOpTest(OpType::Not, "0");

    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runOpTest(OpType::Not, inf);
    runOpTest(OpType::Not, -inf);
    runOpTest(OpType::Not, nan);

    runOpTest(OpType::Not, "true");
    runOpTest(OpType::Not, "false");
    runOpTest(OpType::Not, "TRUE");
    runOpTest(OpType::Not, "FALSE");

    runOpTest(OpType::Not, "00001");
    runOpTest(OpType::Not, "00000");

    runOpTest(OpType::Not, "Infinity");
    runOpTest(OpType::Not, "infinity");
    runOpTest(OpType::Not, "-Infinity");
    runOpTest(OpType::Not, "-infinity");
    runOpTest(OpType::Not, "NaN");
    runOpTest(OpType::Not, "nan");
}

TEST_F(LLVMCodeBuilderTest, Mod)
{
    runOpTest(OpType::Mod, 4, 3);
    runOpTest(OpType::Mod, 3, 3);
    runOpTest(OpType::Mod, 2, 3);
    runOpTest(OpType::Mod, 1, 3);
    runOpTest(OpType::Mod, 0, 3);
    runOpTest(OpType::Mod, -1, 3);
    runOpTest(OpType::Mod, -2, 3);
    runOpTest(OpType::Mod, -3, 3);
    runOpTest(OpType::Mod, -4, 3);
    runOpTest(OpType::Mod, 4.75, 2);
    runOpTest(OpType::Mod, -4.75, 2);
    runOpTest(OpType::Mod, -4.75, -2);
    runOpTest(OpType::Mod, 4.75, -2);
    runOpTest(OpType::Mod, 5, 0);
    runOpTest(OpType::Mod, -5, 0);
    runOpTest(OpType::Mod, -2.5, "Infinity");
    runOpTest(OpType::Mod, -1.2, "-Infinity");
    runOpTest(OpType::Mod, 2.5, "Infinity");
    runOpTest(OpType::Mod, 1.2, "-Infinity");
    runOpTest(OpType::Mod, "Infinity", 2);
    runOpTest(OpType::Mod, "-Infinity", 2);
    runOpTest(OpType::Mod, "Infinity", -2);
    runOpTest(OpType::Mod, "-Infinity", -2);
    runOpTest(OpType::Mod, 3, "NaN");
    runOpTest(OpType::Mod, -3, "NaN");
    runOpTest(OpType::Mod, "NaN", 5);
    runOpTest(OpType::Mod, "NaN", -5);
}

TEST_F(LLVMCodeBuilderTest, Round)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Round, 4.0, 4.0);
    runUnaryNumOpTest(OpType::Round, 3.2, 3.0);
    runUnaryNumOpTest(OpType::Round, 3.5, 4.0);
    runUnaryNumOpTest(OpType::Round, 3.6, 4.0);
    runUnaryNumOpTest(OpType::Round, -2.4, -2.0);
    runUnaryNumOpTest(OpType::Round, -2.5, -2.0);
    runUnaryNumOpTest(OpType::Round, -2.6, -3.0);
    runUnaryNumOpTest(OpType::Round, -0.4, -0.0);
    runUnaryNumOpTest(OpType::Round, -0.5, -0.0);
    runUnaryNumOpTest(OpType::Round, -0.51, -1.0);
    runUnaryNumOpTest(OpType::Round, inf, inf);
    runUnaryNumOpTest(OpType::Round, -inf, -inf);
    runUnaryNumOpTest(OpType::Round, nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Abs)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Abs, 4.0, 4.0);
    runUnaryNumOpTest(OpType::Abs, 3.2, 3.2);
    runUnaryNumOpTest(OpType::Abs, -2.0, 2.0);
    runUnaryNumOpTest(OpType::Abs, -2.5, 2.5);
    runUnaryNumOpTest(OpType::Abs, -2.6, 2.6);
    runUnaryNumOpTest(OpType::Abs, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Abs, -0.0, 0.0);
    runUnaryNumOpTest(OpType::Abs, inf, inf);
    runUnaryNumOpTest(OpType::Abs, -inf, inf);
    runUnaryNumOpTest(OpType::Abs, nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Floor)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Floor, 4.0, 4.0);
    runUnaryNumOpTest(OpType::Floor, 3.2, 3.0);
    runUnaryNumOpTest(OpType::Floor, 3.5, 3.0);
    runUnaryNumOpTest(OpType::Floor, 3.6, 3.0);
    runUnaryNumOpTest(OpType::Floor, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Floor, -0.0, -0.0);
    runUnaryNumOpTest(OpType::Floor, -2.4, -3.0);
    runUnaryNumOpTest(OpType::Floor, -2.5, -3.0);
    runUnaryNumOpTest(OpType::Floor, -2.6, -3.0);
    runUnaryNumOpTest(OpType::Floor, -0.4, -1.0);
    runUnaryNumOpTest(OpType::Floor, -0.5, -1.0);
    runUnaryNumOpTest(OpType::Floor, -0.51, -1.0);
    runUnaryNumOpTest(OpType::Floor, inf, inf);
    runUnaryNumOpTest(OpType::Floor, -inf, -inf);
    runUnaryNumOpTest(OpType::Floor, nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Ceil)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Ceil, 8.0, 8.0);
    runUnaryNumOpTest(OpType::Ceil, 3.2, 4.0);
    runUnaryNumOpTest(OpType::Ceil, 3.5, 4.0);
    runUnaryNumOpTest(OpType::Ceil, 3.6, 4.0);
    runUnaryNumOpTest(OpType::Ceil, 0.4, 1.0);
    runUnaryNumOpTest(OpType::Ceil, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Ceil, -0.0, -0.0);
    runUnaryNumOpTest(OpType::Ceil, -2.4, -2.0);
    runUnaryNumOpTest(OpType::Ceil, -2.5, -2.0);
    runUnaryNumOpTest(OpType::Ceil, -2.6, -2.0);
    runUnaryNumOpTest(OpType::Ceil, -0.4, -0.0);
    runUnaryNumOpTest(OpType::Ceil, -0.5, -0.0);
    runUnaryNumOpTest(OpType::Ceil, -0.51, -0.0);
    runUnaryNumOpTest(OpType::Ceil, inf, inf);
    runUnaryNumOpTest(OpType::Ceil, -inf, -inf);
    runUnaryNumOpTest(OpType::Ceil, nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Sqrt)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Sqrt, 16.0, 4.0);
    runUnaryNumOpTest(OpType::Sqrt, 0.04, 0.2);
    runUnaryNumOpTest(OpType::Sqrt, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Sqrt, -0.0, 0.0);
    runUnaryNumOpTest(OpType::Sqrt, -4.0, -nan); // negative NaN shouldn't be a problem
    runUnaryNumOpTest(OpType::Sqrt, inf, inf);
    runUnaryNumOpTest(OpType::Sqrt, -inf, -nan);
    runUnaryNumOpTest(OpType::Sqrt, nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Sin)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Sin, 30.0, 0.5);
    runUnaryNumOpTest(OpType::Sin, 90.0, 1.0);
    runUnaryNumOpTest(OpType::Sin, 2.8e-9, 0.0);
    runUnaryNumOpTest(OpType::Sin, 2.9e-9, 1e-10);
    runUnaryNumOpTest(OpType::Sin, 570.0, -0.5);
    runUnaryNumOpTest(OpType::Sin, -30.0, -0.5);
    runUnaryNumOpTest(OpType::Sin, -90.0, -1.0);
    runUnaryNumOpTest(OpType::Sin, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Sin, -0.0, 0.0);
    runUnaryNumOpTest(OpType::Sin, inf, -nan); // negative NaN shouldn't be a problem
    runUnaryNumOpTest(OpType::Sin, -inf, -nan);
    runUnaryNumOpTest(OpType::Sin, nan, 0);
}

TEST_F(LLVMCodeBuilderTest, Cos)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Cos, 60.0, 0.5);
    runUnaryNumOpTest(OpType::Cos, 90.0, 0.0);
    runUnaryNumOpTest(OpType::Cos, 600.0, -0.5);
    runUnaryNumOpTest(OpType::Cos, 89.9999999971352, 1e-10);
    runUnaryNumOpTest(OpType::Cos, 89.999999999, 0.0);
    runUnaryNumOpTest(OpType::Cos, -60.0, 0.5);
    runUnaryNumOpTest(OpType::Cos, -90.0, 0.0);
    runUnaryNumOpTest(OpType::Cos, 0.0, 1.0);
    runUnaryNumOpTest(OpType::Cos, -0.0, 1.0);
    runUnaryNumOpTest(OpType::Cos, inf, -nan); // negative NaN shouldn't be a problem
    runUnaryNumOpTest(OpType::Cos, -inf, -nan);
    runUnaryNumOpTest(OpType::Cos, nan, 1.0);
}

TEST_F(LLVMCodeBuilderTest, Tan)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Tan, 45.0, 1.0);
    runUnaryNumOpTest(OpType::Tan, 90.0, inf);
    runUnaryNumOpTest(OpType::Tan, 270.0, -inf);
    runUnaryNumOpTest(OpType::Tan, 450.0, inf);
    runUnaryNumOpTest(OpType::Tan, -90.0, -inf);
    runUnaryNumOpTest(OpType::Tan, -270.0, inf);
    runUnaryNumOpTest(OpType::Tan, -450.0, -inf);
    runUnaryNumOpTest(OpType::Tan, 180.0, 0.0);
    runUnaryNumOpTest(OpType::Tan, -180.0, 0.0);
    runUnaryNumOpTest(OpType::Tan, 2.87e-9, 1e-10);
    runUnaryNumOpTest(OpType::Tan, 2.8647e-9, 0.0);
    runUnaryNumOpTest(OpType::Tan, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Tan, -0.0, 0.0);
    runUnaryNumOpTest(OpType::Tan, inf, -nan); // negative NaN shouldn't be a problem
    runUnaryNumOpTest(OpType::Tan, -inf, -nan);
    runUnaryNumOpTest(OpType::Tan, nan, 0.0);
}

TEST_F(LLVMCodeBuilderTest, Asin)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Asin, 1.0, 90.0);
    runUnaryNumOpTest(OpType::Asin, 0.5, 30.0);
    runUnaryNumOpTest(OpType::Asin, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Asin, -0.0, 0.0);
    runUnaryNumOpTest(OpType::Asin, -0.5, -30.0);
    runUnaryNumOpTest(OpType::Asin, -1.0, -90.0);
    runUnaryNumOpTest(OpType::Asin, 1.1, nan);
    runUnaryNumOpTest(OpType::Asin, -1.2, nan);
    runUnaryNumOpTest(OpType::Asin, inf, nan);
    runUnaryNumOpTest(OpType::Asin, -inf, nan);
    runUnaryNumOpTest(OpType::Asin, nan, 0.0);
}

TEST_F(LLVMCodeBuilderTest, Acos)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Acos, 1.0, 0.0);
    runUnaryNumOpTest(OpType::Acos, 0.5, 60.0);
    runUnaryNumOpTest(OpType::Acos, 0.0, 90.0);
    runUnaryNumOpTest(OpType::Acos, -0.0, 90.0);
    runUnaryNumOpTest(OpType::Acos, -0.5, 120.0);
    runUnaryNumOpTest(OpType::Acos, -1.0, 180.0);
    runUnaryNumOpTest(OpType::Acos, 1.1, nan);
    runUnaryNumOpTest(OpType::Acos, -1.2, nan);
    runUnaryNumOpTest(OpType::Acos, inf, nan);
    runUnaryNumOpTest(OpType::Acos, -inf, nan);
    runUnaryNumOpTest(OpType::Acos, nan, 90.0);
}

TEST_F(LLVMCodeBuilderTest, Atan)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Atan, 1.0, 45.0);
    runUnaryNumOpTest(OpType::Atan, 0.0, 0.0);
    runUnaryNumOpTest(OpType::Atan, -0.0, -0.0);
    runUnaryNumOpTest(OpType::Atan, -1.0, -45.0);
    runUnaryNumOpTest(OpType::Atan, inf, 90.0);
    runUnaryNumOpTest(OpType::Atan, -inf, -90.0);
    runUnaryNumOpTest(OpType::Atan, nan, 0.0);
}

TEST_F(LLVMCodeBuilderTest, Ln)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Ln, std::exp(1.0), 1.0);
    runUnaryNumOpTest(OpType::Ln, std::exp(2.0), 2.0);
    runUnaryNumOpTest(OpType::Ln, std::exp(0.3), 0.3);
    runUnaryNumOpTest(OpType::Ln, 1.0, 0.0);
    runUnaryNumOpTest(OpType::Ln, 0.0, -inf);
    runUnaryNumOpTest(OpType::Ln, -0.0, -inf);
    runUnaryNumOpTest(OpType::Ln, -0.7, -nan); // negative NaN shouldn't be a problem
    runUnaryNumOpTest(OpType::Ln, inf, inf);
    runUnaryNumOpTest(OpType::Ln, -inf, -nan);
    runUnaryNumOpTest(OpType::Ln, nan, -inf);
}

TEST_F(LLVMCodeBuilderTest, Log10)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Log10, 10.0, 1.0);
    runUnaryNumOpTest(OpType::Log10, 1000.0, 3.0);
    runUnaryNumOpTest(OpType::Log10, 0.01, -2.0);
    runUnaryNumOpTest(OpType::Log10, 0.0, -inf);
    runUnaryNumOpTest(OpType::Log10, -0.0, -inf);
    runUnaryNumOpTest(OpType::Log10, -0.7, nan);
    runUnaryNumOpTest(OpType::Log10, inf, inf);
    runUnaryNumOpTest(OpType::Log10, -inf, nan);
    runUnaryNumOpTest(OpType::Log10, nan, -inf);
}

TEST_F(LLVMCodeBuilderTest, Exp)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Exp, 1.0, std::exp(1.0));
    runUnaryNumOpTest(OpType::Exp, 0.5, std::exp(0.5));
    runUnaryNumOpTest(OpType::Exp, 0.0, 1.0);
    runUnaryNumOpTest(OpType::Exp, -0.0, 1.0);
    runUnaryNumOpTest(OpType::Exp, -0.7, std::exp(-0.7));
    runUnaryNumOpTest(OpType::Exp, inf, inf);
    runUnaryNumOpTest(OpType::Exp, -inf, 0.0);
    runUnaryNumOpTest(OpType::Exp, nan, 1.0);
}

TEST_F(LLVMCodeBuilderTest, Exp10)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    runUnaryNumOpTest(OpType::Exp10, 1.0, 10.0);
    runUnaryNumOpTest(OpType::Exp10, 3.0, 1000.0);
    runUnaryNumOpTest(OpType::Exp10, 0.0, 1.0);
    runUnaryNumOpTest(OpType::Exp10, -0.0, 1.0);
    runUnaryNumOpTest(OpType::Exp10, -1.0, 0.1);
    runUnaryNumOpTest(OpType::Exp10, -5.0, 0.00001);
    runUnaryNumOpTest(OpType::Exp10, inf, inf);
    runUnaryNumOpTest(OpType::Exp10, -inf, 0.0);
    runUnaryNumOpTest(OpType::Exp10, nan, 1.0);
}

TEST_F(LLVMCodeBuilderTest, WriteVariable)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalVar1 = std::make_shared<Variable>("", "");
    auto globalVar2 = std::make_shared<Variable>("", "");
    auto globalVar3 = std::make_shared<Variable>("", "");
    stage.addVariable(globalVar1);
    stage.addVariable(globalVar2);
    stage.addVariable(globalVar3);

    auto localVar1 = std::make_shared<Variable>("", "");
    auto localVar2 = std::make_shared<Variable>("", "");
    auto localVar3 = std::make_shared<Variable>("", "");
    sprite.addVariable(localVar1);
    sprite.addVariable(localVar2);
    sprite.addVariable(localVar3);

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(5);
    m_builder->createVariableWrite(globalVar1.get(), v);

    v = m_builder->addConstValue(-23.5);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->createVariableWrite(globalVar2.get(), v);

    v = m_builder->addConstValue("test");
    m_builder->createVariableWrite(globalVar3.get(), v);

    v = m_builder->addConstValue("abc");
    m_builder->createVariableWrite(localVar1.get(), v);

    v = m_builder->addConstValue("hello world");
    v = callConstFuncForType(ValueType::String, v);
    m_builder->createVariableWrite(localVar1.get(), v);

    v = m_builder->addConstValue(false);
    m_builder->createVariableWrite(localVar2.get(), v);

    v = m_builder->addConstValue(true);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->createVariableWrite(localVar3.get(), v);

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());

    ASSERT_EQ(globalVar1->value(), 5);
    ASSERT_EQ(globalVar2->value(), -23.5);
    ASSERT_EQ(globalVar3->value(), "test");
    ASSERT_EQ(localVar1->value(), "hello world");
    ASSERT_EQ(localVar2->value(), false);
    ASSERT_EQ(localVar3->value(), true);
}

TEST_F(LLVMCodeBuilderTest, Select)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    createBuilder(&sprite, true);

    // Number
    CompilerValue *v = m_builder->addConstValue(true);
    v = m_builder->createSelect(v, m_builder->addConstValue(5.8), m_builder->addConstValue(-17.42), Compiler::StaticType::Number);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addConstValue(false);
    v = m_builder->createSelect(v, m_builder->addConstValue(5.8), m_builder->addConstValue(-17.42), Compiler::StaticType::Number);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Bool
    v = m_builder->addConstValue(true);
    v = m_builder->createSelect(v, m_builder->addConstValue(true), m_builder->addConstValue(false), Compiler::StaticType::Bool);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue(false);
    v = m_builder->createSelect(v, m_builder->addConstValue(true), m_builder->addConstValue(false), Compiler::StaticType::Bool);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // String
    v = m_builder->addConstValue(true);
    v = m_builder->createSelect(v, m_builder->addConstValue("hello"), m_builder->addConstValue("world"), Compiler::StaticType::String);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(false);
    v = m_builder->createSelect(v, m_builder->addConstValue("hello"), m_builder->addConstValue("world"), Compiler::StaticType::String);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Different types
    v = m_builder->addConstValue(true);
    v = m_builder->createSelect(v, m_builder->addConstValue("543"), m_builder->addConstValue("true"), Compiler::StaticType::Number);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addConstValue(false);
    v = m_builder->createSelect(v, m_builder->addConstValue("543"), m_builder->addConstValue("true"), Compiler::StaticType::Number);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addConstValue(true);
    v = m_builder->createSelect(v, m_builder->addConstValue(1), m_builder->addConstValue("false"), Compiler::StaticType::Bool);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = m_builder->addConstValue(false);
    v = m_builder->createSelect(v, m_builder->addConstValue(1), m_builder->addConstValue("false"), Compiler::StaticType::Bool);
    m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    static const std::string expected =
        "5.8\n"
        "-17.42\n"
        "1\n"
        "0\n"
        "hello\n"
        "world\n"
        "543\n"
        "0\n"
        "1\n"
        "0\n";

    auto code = m_builder->finalize();
    testing::internal::CaptureStdout();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, ReadVariable)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalVar1 = std::make_shared<Variable>("", "", 87);
    auto globalVar2 = std::make_shared<Variable>("", "", 6.4);
    auto globalVar3 = std::make_shared<Variable>("", "", "abc");
    stage.addVariable(globalVar1);
    stage.addVariable(globalVar2);
    stage.addVariable(globalVar3);

    auto localVar1 = std::make_shared<Variable>("", "", false);
    auto localVar2 = std::make_shared<Variable>("", "", true);
    auto localVar3 = std::make_shared<Variable>("", "", "test");
    sprite.addVariable(localVar1);
    sprite.addVariable(localVar2);
    sprite.addVariable(localVar3);

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addVariableValue(globalVar1.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(globalVar2.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(globalVar3.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar1.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar2.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar3.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected;
    expected += globalVar1->value().toString() + '\n';
    expected += globalVar2->value().toString() + '\n';
    expected += globalVar3->value().toString() + '\n';
    expected += localVar1->value().toString() + '\n';
    expected += localVar2->value().toString() + '\n';
    expected += localVar3->value().toString() + '\n';

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, ClearList)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    std::unordered_map<List *, std::string> strings;

    auto globalList1 = std::make_shared<List>("", "");
    auto globalList2 = std::make_shared<List>("", "");
    auto globalList3 = std::make_shared<List>("", "");
    stage.addList(globalList1);
    stage.addList(globalList2);
    stage.addList(globalList3);

    auto localList1 = std::make_shared<List>("", "");
    auto localList2 = std::make_shared<List>("", "");
    auto localList3 = std::make_shared<List>("", "");
    sprite.addList(localList1);
    sprite.addList(localList2);
    sprite.addList(localList3);

    globalList1->append(1);
    globalList1->append(2);
    globalList1->append(3);
    strings[globalList1.get()] = globalList1->toString();

    globalList2->append("Lorem");
    globalList2->append("ipsum");
    globalList2->append(-4.52);
    strings[globalList2.get()] = globalList2->toString();

    globalList3->append(true);
    globalList3->append(false);
    globalList3->append(true);
    strings[globalList3.get()] = globalList3->toString();

    localList1->append("dolor");
    localList1->append("sit");
    localList1->append("amet");
    strings[localList1.get()] = localList1->toString();

    localList2->append(10);
    localList2->append(9.8);
    localList2->append(true);
    strings[localList2.get()] = localList2->toString();

    localList3->append("test");
    localList3->append(1.2);
    localList3->append(false);
    strings[localList3.get()] = localList3->toString();

    createBuilder(&sprite, true);

    m_builder->createListClear(globalList1.get());
    m_builder->createListClear(globalList3.get());
    m_builder->createListClear(localList1.get());
    m_builder->createListClear(localList2.get());

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());

    ASSERT_TRUE(globalList1->empty());
    ASSERT_EQ(globalList2->toString(), strings[globalList2.get()]);
    ASSERT_TRUE(globalList3->empty());

    ASSERT_TRUE(localList1->empty());
    ASSERT_TRUE(localList2->empty());
    ASSERT_EQ(localList3->toString(), strings[localList3.get()]);
}

TEST_F(LLVMCodeBuilderTest, RemoveFromList)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    std::unordered_map<List *, std::string> strings;

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");
    strings[localList.get()] = localList->toString();

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(1);
    m_builder->createListRemove(globalList.get(), v);

    v = m_builder->addConstValue(3);
    m_builder->createListRemove(localList.get(), v);

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "13");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor");
}

TEST_F(LLVMCodeBuilderTest, AppendToList)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    std::unordered_map<List *, std::string> strings;

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");
    strings[localList.get()] = localList->toString();

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(1);
    m_builder->createListAppend(globalList.get(), v);

    v = m_builder->addConstValue("test");
    m_builder->createListAppend(globalList.get(), v);

    v = m_builder->addConstValue(3);
    m_builder->createListAppend(localList.get(), v);

    m_builder->createListClear(localList.get());

    v = m_builder->addConstValue(true);
    m_builder->createListAppend(localList.get(), v);

    v = m_builder->addConstValue(false);
    m_builder->createListAppend(localList.get(), v);

    v = m_builder->addConstValue("hello world");
    m_builder->createListAppend(localList.get(), v);

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "1 2 3 1 test");
    ASSERT_EQ(localList->toString(), "true false hello world");
}

TEST_F(LLVMCodeBuilderTest, InsertToList)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    std::unordered_map<List *, std::string> strings;

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");
    strings[localList.get()] = localList->toString();

    createBuilder(&sprite, true);

    CompilerValue *v1 = m_builder->addConstValue(2);
    CompilerValue *v2 = m_builder->addConstValue(1);
    m_builder->createListInsert(globalList.get(), v1, v2);

    v1 = m_builder->addConstValue(3);
    v2 = m_builder->addConstValue("test");
    m_builder->createListInsert(globalList.get(), v1, v2);

    v1 = m_builder->addConstValue(0);
    v2 = m_builder->addConstValue(3);
    m_builder->createListInsert(localList.get(), v1, v2);

    m_builder->createListClear(localList.get());

    v1 = m_builder->addConstValue(0);
    v2 = m_builder->addConstValue(true);
    m_builder->createListInsert(localList.get(), v1, v2);

    v1 = m_builder->addConstValue(0);
    v2 = m_builder->addConstValue(false);
    m_builder->createListInsert(localList.get(), v1, v2);

    v1 = m_builder->addConstValue(1);
    v2 = m_builder->addConstValue("hello world");
    m_builder->createListInsert(localList.get(), v1, v2);

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "1 2 1 test 3");
    ASSERT_EQ(localList->toString(), "false hello world true");
}

TEST_F(LLVMCodeBuilderTest, ListReplace)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    std::unordered_map<List *, std::string> strings;

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");
    strings[localList.get()] = localList->toString();

    createBuilder(&sprite, true);

    CompilerValue *v1 = m_builder->addConstValue(2);
    CompilerValue *v2 = m_builder->addConstValue(1);
    m_builder->createListReplace(globalList.get(), v1, v2);

    v1 = m_builder->addConstValue(1);
    v2 = m_builder->addConstValue("test");
    m_builder->createListReplace(globalList.get(), v1, v2);

    v1 = m_builder->addConstValue(0);
    v2 = m_builder->addConstValue(3);
    m_builder->createListReplace(localList.get(), v1, v2);

    v1 = m_builder->addConstValue(2);
    v2 = m_builder->addConstValue(true);
    m_builder->createListReplace(localList.get(), v1, v2);

    v1 = m_builder->addConstValue(3);
    v2 = m_builder->addConstValue("hello world");
    m_builder->createListReplace(localList.get(), v1, v2);

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "1 test 1");
    ASSERT_EQ(localList->toString(), "3 ipsum true hello world");
}

TEST_F(LLVMCodeBuilderTest, GetListContents)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    std::unordered_map<List *, std::string> strings;

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");
    strings[localList.get()] = localList->toString();

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addListContents(globalList.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListContents(localList.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    static const std::string expected =
        "123\n"
        "Lorem ipsum dolor sit\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "123");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, GetListItem)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    std::unordered_map<List *, std::string> strings;

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");
    strings[localList.get()] = localList->toString();

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(2);
    v = m_builder->addListItem(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    CompilerValue *v1 = m_builder->addConstValue(1);
    CompilerValue *v2 = m_builder->addConstValue("test");
    m_builder->createListReplace(globalList.get(), v1, v2);

    v = m_builder->addConstValue(0);
    v = m_builder->addListItem(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(0);
    v = m_builder->addListItem(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(2);
    v = m_builder->addListItem(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(3);
    v = m_builder->addListItem(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    static const std::string expected =
        "3\n"
        "1\n"
        "Lorem\n"
        "dolor\n"
        "sit\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "1 test 3");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, GetListSize)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addListSize(globalList.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListSize(localList.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    static const std::string expected =
        "3\n"
        "4\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "123");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, GetListItemIndex)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(2);
    v = m_builder->addListItemIndex(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(1);
    v = m_builder->addListItemIndex(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(0);
    v = m_builder->addListItemIndex(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    CompilerValue *v1 = m_builder->addConstValue(1);
    CompilerValue *v2 = m_builder->addConstValue("test");
    m_builder->createListReplace(globalList.get(), v1, v2);

    v = m_builder->addConstValue(2);
    v = m_builder->addListItemIndex(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(1);
    v = m_builder->addListItemIndex(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("test");
    v = m_builder->addListItemIndex(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("abc");
    v = m_builder->addListItemIndex(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("doLor");
    v = m_builder->addListItemIndex(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(true);
    v = m_builder->addListItemIndex(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("site");
    v = m_builder->addListItemIndex(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    static const std::string expected =
        "1\n"
        "0\n"
        "-1\n"
        "-1\n"
        "0\n"
        "1\n"
        "-1\n"
        "2\n"
        "-1\n"
        "-1\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "1 test 3");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, ListContainsItem)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList->append("Lorem");
    localList->append("ipsum");
    localList->append("dolor");
    localList->append("sit");

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(2);
    v = m_builder->addListContains(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(1);
    v = m_builder->addListContains(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(0);
    v = m_builder->addListContains(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    CompilerValue *v1 = m_builder->addConstValue(1);
    CompilerValue *v2 = m_builder->addConstValue("test");
    m_builder->createListReplace(globalList.get(), v1, v2);

    v = m_builder->addConstValue(2);
    v = m_builder->addListContains(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(1);
    v = m_builder->addListContains(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("test");
    v = m_builder->addListContains(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("abc");
    v = m_builder->addListContains(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("doLor");
    v = m_builder->addListContains(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(true);
    v = m_builder->addListContains(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue("site");
    v = m_builder->addListContains(localList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    static const std::string expected =
        "true\n"
        "true\n"
        "false\n"
        "false\n"
        "true\n"
        "true\n"
        "false\n"
        "true\n"
        "false\n"
        "false\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "1 test 3");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, Yield)
{
    auto build = [this]() {
        m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

        CompilerValue *v = m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        m_builder->yield();

        v = m_builder->addConstValue("1");
        v = m_builder->addFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        CompilerValue *v1 = m_builder->addConstValue("2");
        CompilerValue *v2 = m_builder->addConstValue(3);
        m_builder->addFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String }, { v, v1, v2 });

        v = m_builder->addConstValue("test");
        v1 = m_builder->addConstValue("4");
        v2 = m_builder->addConstValue("5");
        v = m_builder->addFunctionCall(
            "test_function_3_args_ret",
            Compiler::StaticType::String,
            { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String },
            { v, v1, v2 });
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
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

TEST_F(LLVMCodeBuilderTest, VariablesAfterSuspend)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "", 87);
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "", "test");
    sprite.addVariable(localVar);

    createBuilder(&sprite, false);

    CompilerValue *v = m_builder->addConstValue(12.5);
    m_builder->createVariableWrite(globalVar.get(), v);

    v = m_builder->addConstValue(true);
    m_builder->createVariableWrite(localVar.get(), v);

    m_builder->yield();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "hello world\n"
        "-4.8\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());
    ASSERT_FALSE(code->isFinished(ctx.get()));

    globalVar->setValue("hello world");
    localVar->setValue(-4.8);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, ListsAfterSuspend)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalList1 = std::make_shared<List>("", "");
    stage.addList(globalList1);

    auto globalList2 = std::make_shared<List>("", "");
    stage.addList(globalList2);

    auto localList1 = std::make_shared<List>("", "");
    sprite.addList(localList1);

    auto localList2 = std::make_shared<List>("", "");
    sprite.addList(localList2);

    createBuilder(&sprite, false);

    m_builder->createListClear(globalList1.get());
    m_builder->createListClear(globalList2.get());
    m_builder->createListClear(localList1.get());
    m_builder->createListClear(localList2.get());

    m_builder->createListAppend(globalList1.get(), m_builder->addConstValue(1));
    m_builder->createListAppend(globalList1.get(), m_builder->addConstValue(2));
    m_builder->createListAppend(globalList1.get(), m_builder->addConstValue(3));

    m_builder->createListAppend(globalList2.get(), m_builder->addConstValue(1));
    m_builder->createListAppend(globalList2.get(), m_builder->addConstValue(2));
    m_builder->createListAppend(globalList2.get(), m_builder->addConstValue(3));

    m_builder->createListReplace(globalList2.get(), m_builder->addConstValue(1), m_builder->addConstValue(12.5));

    m_builder->createListInsert(localList1.get(), m_builder->addConstValue(0), m_builder->addConstValue("Lorem"));
    m_builder->createListInsert(localList1.get(), m_builder->addConstValue(0), m_builder->addConstValue("ipsum"));

    m_builder->createListInsert(localList2.get(), m_builder->addConstValue(0), m_builder->addConstValue(true));
    m_builder->createListInsert(localList2.get(), m_builder->addConstValue(0), m_builder->addConstValue(false));

    m_builder->yield();

    CompilerValue *v = m_builder->addListItem(globalList1.get(), m_builder->addConstValue(1));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListItem(globalList1.get(), m_builder->addConstValue(2));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListItem(globalList2.get(), m_builder->addConstValue(1));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListItem(globalList2.get(), m_builder->addConstValue(0));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListItem(localList1.get(), m_builder->addConstValue(0));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListItem(localList1.get(), m_builder->addConstValue(1));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListItem(localList2.get(), m_builder->addConstValue(0));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "2\n"
        "test\n"
        "12.5\n"
        "false\n"
        "ipsum\n"
        "-5.48\n"
        "hello\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    code->run(ctx.get());
    ASSERT_FALSE(code->isFinished(ctx.get()));

    globalList1->replace(2, "test");
    globalList2->replace(0, false);
    localList1->replace(1, -5.48);
    localList2->replace(0, "hello");

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, IfStatement)
{
    createBuilder(true);

    // Without else branch (const condition)
    CompilerValue *v = m_builder->addConstValue("true");
    m_builder->beginIfStatement(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endIf();

    v = m_builder->addConstValue("false");
    m_builder->beginIfStatement(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endIf();

    // Without else branch (condition returned by function)
    CompilerValue *v1 = m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    CompilerValue *v2 = m_builder->addConstValue("no_args_output");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    v1 = m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = m_builder->addConstValue("");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(1);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    // With else branch (const condition)
    v = m_builder->addConstValue("true");
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(2);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(3);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    v = m_builder->addConstValue("false");
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(4);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(5);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    // With else branch (condition returned by function)
    v1 = m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = m_builder->addConstValue("no_args_output");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(6);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(7);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    v1 = m_builder->addFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = m_builder->addConstValue("");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(8);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(9);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    // Nested 1
    v = m_builder->addConstValue(true);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue(false);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(0);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->beginElseBranch();
        {
            v = m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

            v = m_builder->addConstValue(false);
            m_builder->beginIfStatement(v);
            m_builder->beginElseBranch();
            {
                v = m_builder->addConstValue(2);
                m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            }
            m_builder->endIf();
        }
        m_builder->endIf();
    }
    m_builder->beginElseBranch();
    {
        v = m_builder->addConstValue(true);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(3);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->beginElseBranch();
        {
            v = m_builder->addConstValue(4);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->endIf();
    }
    m_builder->endIf();

    // Nested 2
    v = m_builder->addConstValue(false);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue(false);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(5);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->beginElseBranch();
        {
            v = m_builder->addConstValue(6);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->endIf();
    }
    m_builder->beginElseBranch();
    {
        v = m_builder->addConstValue(true);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(7);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
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

TEST_F(LLVMCodeBuilderTest, IfStatementVariables)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "", "test");
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "", 87);
    sprite.addVariable(localVar);

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(12.5);
    m_builder->createVariableWrite(globalVar.get(), v);

    v = m_builder->addConstValue(true);
    m_builder->createVariableWrite(localVar.get(), v);

    v = m_builder->addConstValue(true);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createVariableWrite(globalVar.get(), v);
    }
    m_builder->endIf();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(12.5);
    m_builder->createVariableWrite(globalVar.get(), v);

    v = m_builder->addConstValue(false);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createVariableWrite(globalVar.get(), v);

        v = m_builder->addConstValue(0);
        m_builder->createVariableWrite(localVar.get(), v);
    }
    m_builder->endIf();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(true);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue(true);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(true);
            m_builder->createVariableWrite(globalVar.get(), v);
        }
        m_builder->endIf();

        v = m_builder->addConstValue(false);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(-8.2);
            m_builder->createVariableWrite(localVar.get(), v);
        }
        m_builder->endIf();
    }
    m_builder->endIf();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "hello world\n"
        "12.5\n"
        "true\n"
        "true\n"
        "true\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, IfStatementLists)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalList1 = std::make_shared<List>("", "");
    stage.addList(globalList1);

    auto globalList2 = std::make_shared<List>("", "");
    stage.addList(globalList2);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    createBuilder(&sprite, false);

    auto resetLists = [this, globalList1, globalList2, localList]() {
        m_builder->createListClear(globalList1.get());
        m_builder->createListClear(globalList2.get());
        m_builder->createListClear(localList.get());

        m_builder->createListAppend(globalList1.get(), m_builder->addConstValue(1));
        m_builder->createListAppend(globalList1.get(), m_builder->addConstValue(2));

        m_builder->createListAppend(globalList2.get(), m_builder->addConstValue("hello"));
        m_builder->createListAppend(globalList2.get(), m_builder->addConstValue("world"));

        m_builder->createListAppend(localList.get(), m_builder->addConstValue(false));
        m_builder->createListAppend(localList.get(), m_builder->addConstValue(true));
    };

    auto checkLists = [this, globalList1, globalList2, localList]() {
        CompilerValue *v = m_builder->addListItem(globalList1.get(), m_builder->addConstValue(0));
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addListItem(globalList2.get(), m_builder->addConstValue(1));
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addListItem(localList.get(), m_builder->addConstValue(0));
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    };

    // if (true)
    resetLists();

    CompilerValue *v, *v1, *v2;
    v = m_builder->addConstValue(true);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createListAppend(globalList1.get(), v);

        v1 = m_builder->addConstValue(0);
        v2 = m_builder->addConstValue(8.5);
        m_builder->createListInsert(globalList2.get(), v1, v2);

        v1 = m_builder->addConstValue(1);
        v2 = m_builder->addConstValue(-4.25);
        m_builder->createListReplace(localList.get(), v1, v2);
    }
    m_builder->endIf();

    checkLists();

    // if (false)
    resetLists();

    v = m_builder->addConstValue(false);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createListAppend(globalList1.get(), v);

        v1 = m_builder->addConstValue(0);
        v2 = m_builder->addConstValue(8.5);
        m_builder->createListInsert(globalList2.get(), v1, v2);

        v1 = m_builder->addConstValue(1);
        v2 = m_builder->addConstValue(-4.25);
        m_builder->createListReplace(localList.get(), v1, v2);
    }
    m_builder->endIf();

    checkLists();

    // if (true) { if (true) { ... }; if (false) { ... } }
    resetLists();

    v = m_builder->addConstValue(true);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue(true);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue("hello world");
            m_builder->createListAppend(globalList1.get(), v);

            v1 = m_builder->addConstValue(0);
            v2 = m_builder->addConstValue(8.5);
            m_builder->createListInsert(globalList2.get(), v1, v2);
        }
        m_builder->endIf();

        v = m_builder->addConstValue(false);
        m_builder->beginIfStatement(v);
        {
            v1 = m_builder->addConstValue(1);
            v2 = m_builder->addConstValue(-4.25);
            m_builder->createListReplace(localList.get(), v1, v2);
        }
        m_builder->endIf();
    }
    m_builder->endIf();

    checkLists();

    std::string expected =
        "1\n"
        "hello\n"
        "false\n"
        "1\n"
        "world\n"
        "false\n"
        "1\n"
        "hello\n"
        "false\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, RepeatLoop)
{
    createBuilder(true);

    // Const count
    CompilerValue *v = m_builder->addConstValue("-5");
    m_builder->beginRepeatLoop(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    v = m_builder->addConstValue(0);
    m_builder->beginRepeatLoop(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    v = m_builder->addConstValue(3);
    m_builder->beginRepeatLoop(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    v = m_builder->addConstValue("2.4");
    m_builder->beginRepeatLoop(v);
    v = m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endLoop();

    v = m_builder->addConstValue("2.5");
    m_builder->beginRepeatLoop(v);
    v = m_builder->addConstValue(1);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endLoop();

    // Count returned by function
    v = m_builder->addConstValue(2);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->beginRepeatLoop(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    // Nested
    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    {
        v = m_builder->addConstValue(2);
        m_builder->beginRepeatLoop(v);
        {
            v = m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->endLoop();

        v = m_builder->addConstValue(2);
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addConstValue(3);
        m_builder->beginRepeatLoop(v);
        {
            v = m_builder->addConstValue(3);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
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

    v = m_builder->addConstValue(3);
    m_builder->beginRepeatLoop(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
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

    v = m_builder->addConstValue(0); // don't yield
    m_builder->beginRepeatLoop(v);
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
    CompilerValue *v = m_builder->addConstValue("false");
    m_builder->beginWhileLoop(v);
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    m_builder->beginLoopCondition();
    v = m_builder->addConstValue(false);
    m_builder->beginWhileLoop(v);
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    // Condition returned by function
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    m_builder->beginLoopCondition();
    CompilerValue *v1 = m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    CompilerValue *v2 = m_builder->addConstValue(2);
    v = m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    m_builder->beginWhileLoop(v);
    v = m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    // Nested
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    m_builder->beginLoopCondition();
    v1 = m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    v2 = m_builder->addConstValue(3);
    v = m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    m_builder->beginWhileLoop(v);
    {
        m_builder->beginLoopCondition();
        v1 = m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
        v2 = m_builder->addConstValue(3);
        v = m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
        m_builder->beginWhileLoop(v);
        {
            v = m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
        }
        m_builder->endLoop();

        v = m_builder->addConstValue(2);
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        m_builder->beginLoopCondition();
        v = m_builder->addConstValue(false);
        m_builder->beginWhileLoop(v);
        {
            m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
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
    v = m_builder->addConstValue(true);
    m_builder->beginWhileLoop(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
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
    CompilerValue *v = m_builder->addConstValue("true");
    m_builder->beginRepeatUntilLoop(v);
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    m_builder->beginLoopCondition();
    v = m_builder->addConstValue(true);
    m_builder->beginRepeatUntilLoop(v);
    m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    // Condition returned by function
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    m_builder->beginLoopCondition();
    CompilerValue *v1 = m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    CompilerValue *v2 = m_builder->addConstValue(2);
    v = m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    v = m_builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
    m_builder->beginRepeatUntilLoop(v);
    v = m_builder->addConstValue(0);
    m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    // Nested
    m_builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    m_builder->beginLoopCondition();
    v1 = m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    v2 = m_builder->addConstValue(3);
    v = m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    v = m_builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
    m_builder->beginRepeatUntilLoop(v);
    {
        m_builder->beginLoopCondition();
        v1 = m_builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
        v2 = m_builder->addConstValue(3);
        v = m_builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
        v = m_builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
        m_builder->beginRepeatUntilLoop(v);
        {
            v = m_builder->addConstValue(1);
            m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
        }
        m_builder->endLoop();

        v = m_builder->addConstValue(2);
        m_builder->addFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        m_builder->beginLoopCondition();
        v = m_builder->addConstValue(true);
        m_builder->beginRepeatUntilLoop(v);
        {
            m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
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
    v = m_builder->addConstValue(false);
    m_builder->beginRepeatUntilLoop(v);
    m_builder->addFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
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

TEST_F(LLVMCodeBuilderTest, LoopVariables)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "", "test");
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "", 87);
    auto counter1 = std::make_shared<Variable>("", "");
    auto counter2 = std::make_shared<Variable>("", "");
    sprite.addVariable(localVar);
    sprite.addVariable(counter1);
    sprite.addVariable(counter2);

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(12.5);
    m_builder->createVariableWrite(globalVar.get(), v);

    v = m_builder->addConstValue(true);
    m_builder->createVariableWrite(localVar.get(), v);

    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createVariableWrite(globalVar.get(), v);
    }
    m_builder->endLoop();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(12.5);
    m_builder->createVariableWrite(globalVar.get(), v);

    v = m_builder->addConstValue(0);
    m_builder->beginRepeatLoop(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createVariableWrite(globalVar.get(), v);

        v = m_builder->addConstValue(0);
        m_builder->createVariableWrite(localVar.get(), v);
    }
    m_builder->endLoop();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(0);
    m_builder->createVariableWrite(counter1.get(), v);

    m_builder->beginLoopCondition();
    CompilerValue *v1 = m_builder->addVariableValue(counter1.get());
    CompilerValue *v2 = m_builder->addConstValue(5);
    v = m_builder->createCmpLT(v1, v2);
    m_builder->beginWhileLoop(v);
    {
        v = m_builder->addConstValue(0);
        m_builder->createVariableWrite(counter2.get(), v);

        m_builder->beginLoopCondition();
        v1 = m_builder->addVariableValue(counter2.get());
        v2 = m_builder->addConstValue(3);
        v = m_builder->createCmpEQ(v1, v2);
        m_builder->beginRepeatUntilLoop(v);
        {
            v = m_builder->addConstValue(true);
            m_builder->createVariableWrite(globalVar.get(), v);

            v1 = m_builder->addVariableValue(counter2.get());
            v2 = m_builder->addConstValue(1);
            v = m_builder->createAdd(v1, v2);
            m_builder->createVariableWrite(counter2.get(), v);
        }
        m_builder->endLoop();

        m_builder->beginLoopCondition();
        v = m_builder->addConstValue(false);
        m_builder->beginWhileLoop(v);
        {
            v = m_builder->addConstValue(-8.2);
            m_builder->createVariableWrite(localVar.get(), v);
        }
        m_builder->endLoop();

        v1 = m_builder->addVariableValue(counter1.get());
        v2 = m_builder->addConstValue(1);
        v = m_builder->createAdd(v1, v2);
        m_builder->createVariableWrite(counter1.get(), v);
    }
    m_builder->endLoop();

    m_builder->beginLoopCondition();
    v = m_builder->addConstValue(true);
    m_builder->beginRepeatUntilLoop(v);
    {
        v = m_builder->addConstValue(-8.2);
        m_builder->createVariableWrite(localVar.get(), v);
    }
    m_builder->endLoop();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addVariableValue(localVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "hello world\n"
        "12.5\n"
        "true\n"
        "true\n"
        "true\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, LoopLists)
{
    EngineMock engine;
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, stage()).WillRepeatedly(Return(&stage));

    auto globalList1 = std::make_shared<List>("", "");
    stage.addList(globalList1);

    auto globalList2 = std::make_shared<List>("", "");
    stage.addList(globalList2);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    auto counter1 = std::make_shared<Variable>("", "");
    auto counter2 = std::make_shared<Variable>("", "");
    sprite.addVariable(counter1);
    sprite.addVariable(counter2);

    createBuilder(&sprite, true);

    auto resetLists = [this, globalList1, globalList2, localList]() {
        m_builder->createListClear(globalList1.get());
        m_builder->createListClear(globalList2.get());
        m_builder->createListClear(localList.get());

        m_builder->createListAppend(globalList1.get(), m_builder->addConstValue(1));
        m_builder->createListAppend(globalList1.get(), m_builder->addConstValue(2));

        m_builder->createListAppend(globalList2.get(), m_builder->addConstValue("hello"));
        m_builder->createListAppend(globalList2.get(), m_builder->addConstValue("world"));

        m_builder->createListAppend(localList.get(), m_builder->addConstValue(false));
        m_builder->createListAppend(localList.get(), m_builder->addConstValue(true));
    };

    auto checkLists = [this, globalList1, globalList2, localList]() {
        CompilerValue *v = m_builder->addListItem(globalList1.get(), m_builder->addConstValue(0));
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addListItem(globalList2.get(), m_builder->addConstValue(1));
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addListItem(localList.get(), m_builder->addConstValue(0));
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    };

    // repeat (2)
    resetLists();

    CompilerValue *v, *v1, *v2;
    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createListAppend(globalList1.get(), v);

        v1 = m_builder->addConstValue(0);
        v2 = m_builder->addConstValue(8.5);
        m_builder->createListInsert(globalList2.get(), v1, v2);

        v1 = m_builder->addConstValue(1);
        v2 = m_builder->addConstValue(-4.25);
        m_builder->createListReplace(localList.get(), v1, v2);
    }
    m_builder->endLoop();

    checkLists();

    // repeat(0)
    resetLists();

    v = m_builder->addConstValue(0);
    m_builder->beginRepeatLoop(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createListAppend(globalList1.get(), v);

        v1 = m_builder->addConstValue(0);
        v2 = m_builder->addConstValue(8.5);
        m_builder->createListInsert(globalList2.get(), v1, v2);

        v1 = m_builder->addConstValue(1);
        v2 = m_builder->addConstValue(-4.25);
        m_builder->createListReplace(localList.get(), v1, v2);
    }
    m_builder->endLoop();

    checkLists();

    // while (counter1 < 5) { ... until (counter2 == 3) {... counter2++ }; while (false) { ... }; counter1++ }
    resetLists();

    v = m_builder->addConstValue(0);
    m_builder->createVariableWrite(counter1.get(), v);

    m_builder->beginLoopCondition();
    v1 = m_builder->addVariableValue(counter1.get());
    v2 = m_builder->addConstValue(5);
    v = m_builder->createCmpLT(v1, v2);
    m_builder->beginWhileLoop(v);
    {
        v = m_builder->addConstValue(0);
        m_builder->createVariableWrite(counter2.get(), v);

        m_builder->beginLoopCondition();
        v1 = m_builder->addVariableValue(counter2.get());
        v2 = m_builder->addConstValue(3);
        v = m_builder->createCmpEQ(v1, v2);
        m_builder->beginRepeatUntilLoop(v);
        {
            v1 = m_builder->addConstValue(0);
            v2 = m_builder->addConstValue(8.5);
            m_builder->createListInsert(globalList2.get(), v1, v2);

            v1 = m_builder->addConstValue(1);
            v2 = m_builder->addConstValue(-4.25);
            m_builder->createListReplace(localList.get(), v1, v2);

            v1 = m_builder->addVariableValue(counter2.get());
            v2 = m_builder->addConstValue(1);
            v = m_builder->createAdd(v1, v2);
            m_builder->createVariableWrite(counter2.get(), v);
        }
        m_builder->endLoop();

        m_builder->beginLoopCondition();
        v = m_builder->addConstValue(false);
        m_builder->beginWhileLoop(v);
        {
            v = m_builder->addConstValue("hello world");
            m_builder->createListAppend(globalList1.get(), v);
        }
        m_builder->endLoop();

        v1 = m_builder->addVariableValue(counter1.get());
        v2 = m_builder->addConstValue(1);
        v = m_builder->createAdd(v1, v2);
        m_builder->createVariableWrite(counter1.get(), v);
    }
    m_builder->endLoop();

    checkLists();

    // until (true)
    resetLists();

    m_builder->beginLoopCondition();
    v = m_builder->addConstValue(true);
    m_builder->beginRepeatUntilLoop(v);
    {
        v = m_builder->addConstValue("hello world");
        m_builder->createListAppend(globalList1.get(), v);

        v1 = m_builder->addConstValue(0);
        v2 = m_builder->addConstValue(8.5);
        m_builder->createListInsert(globalList2.get(), v1, v2);

        v1 = m_builder->addConstValue(1);
        v2 = m_builder->addConstValue(-4.25);
        m_builder->createListReplace(localList.get(), v1, v2);
    }
    m_builder->endLoop();

    checkLists();

    std::string expected =
        "1\n"
        "8.5\n"
        "false\n"
        "1\n"
        "world\n"
        "false\n"
        "1\n"
        "8.5\n"
        "false\n"
        "1\n"
        "world\n"
        "false\n";

    auto code = m_builder->finalize();
    auto ctx = code->createExecutionContext(&sprite);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}
