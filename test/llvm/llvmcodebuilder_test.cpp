#include <scratchcpp/value.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/blockprototype.h>
#include <scratchcpp/compilerconstant.h>
#include <engine/internal/llvm/llvmcodebuilder.h>
#include <engine/internal/llvm/llvmcompilercontext.h>
#include <gmock/gmock.h>
#include <targetmock.h>
#include <enginemock.h>
#include <randomgeneratormock.h>

#include "llvmtestutils.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::Eq;

class LLVMCodeBuilderTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMCodeBuilderTest, FunctionCalls)
{
    static const std::vector<bool> warpList = { false, true };

    for (bool warp : warpList) {
        LLVMCodeBuilder *builder = m_utils.createBuilder(warp);

        builder->addFunctionCall("test_empty_function", Compiler::StaticType::Void, {}, {});

        CompilerValue *v = builder->addConstValue("test");
        builder->addFunctionCallWithCtx("test_ctx_function", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

        v = builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
        builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = builder->addConstValue("1");
        v = builder->addTargetFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        CompilerValue *v1 = builder->addConstValue("2");
        CompilerValue *v2 = builder->addConstValue("3");
        builder->addTargetFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String }, { v, v1, v2 });

        v = builder->addConstValue("test");
        v1 = builder->addConstValue("4");
        v2 = builder->addConstValue("5");
        v = builder->addTargetFunctionCall(
            "test_function_3_args_ret",
            Compiler::StaticType::String,
            { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String },
            { v, v1, v2 });
        builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = builder->addConstValue(123);
        v = builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { v });
        builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = builder->addConstValue(true);
        v = builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
        builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

        v = builder->addConstValue(321.5);
        builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });
        v = builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { v });
        builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

        v = builder->addConstValue("test");
        builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });
        v = builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

        v = builder->addConstValue(true);
        builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });
        v = builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
        builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

        auto code = builder->build();
        Script script(&m_utils.target(), nullptr, nullptr);
        script.setCode(code);
        Thread thread(&m_utils.target(), nullptr, &script);
        auto ctx = code->createExecutionContext(&thread);

        std::stringstream s;
        s << ctx.get();
        std::string ctxPtr = s.str();

        const std::string expected =
            "empty\n" + ctxPtr +
            "\n"
            "test\n"
            "no_args\n"
            "no_args_ret\n"
            "1_arg no_args_output\n"
            "1_arg_ret 1\n"
            "3_args 1_arg_output 2 3\n"
            "3_args test 4 5\n"
            "1_arg 3_args_output\n"
            "123\n"
            "1\n"
            "321.5\n"
            "321.5\n"
            "test\n"
            "test\n"
            "true\n"
            "true\n";

        EXPECT_CALL(m_utils.target(), isStage()).Times(7);
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(LLVMCodeBuilderTest, FunctionCallsWithPointers)
{
    LLVMCodeBuilder *builder = m_utils.createBuilder(true);

    int var = 12;
    CompilerValue *v = builder->addConstValue(&var);
    v = builder->addTargetFunctionCall("test_function_1_ptr_arg_ret", Compiler::StaticType::Pointer, { Compiler::StaticType::Pointer }, { v });

    builder->addFunctionCall("test_print_pointer", Compiler::StaticType::Void, { Compiler::StaticType::Pointer }, { v });

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    std::stringstream s;
    s << &m_utils.target();
    std::string ptr = s.str();

    const std::string expected = "1_arg_ret 12\n" + ptr + "\n";

    EXPECT_CALL(m_utils.target(), isStage());
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_TRUE(code->isFinished(ctx.get()));
}

TEST_F(LLVMCodeBuilderTest, ConstCasting)
{
    LLVMCodeBuilder *builder = m_utils.createBuilder(true);

    CompilerValue *v = builder->addConstValue(5.2);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });
    v = builder->addConstValue("-24.156");
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addConstValue(true);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue(0);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue("false");
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue("123");
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("hello world");
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

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
    LLVMCodeBuilder *builder = m_utils.createBuilder(true);

    // Number -> number
    CompilerValue *v = builder->addConstValue(5.2);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Number -> bool
    v = builder->addConstValue(-24.156);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue(0);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // Number -> string
    v = builder->addConstValue(59.8);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Bool -> number
    v = builder->addConstValue(true);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addConstValue(false);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Bool -> bool
    v = builder->addConstValue(true);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue(false);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // Bool -> string
    v = builder->addConstValue(true);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(false);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // String -> number
    v = builder->addConstValue("5.2");
    v = m_utils.callConstFuncForType(ValueType::String, v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // String -> bool
    v = builder->addConstValue("abc");
    v = m_utils.callConstFuncForType(ValueType::String, v);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue("false");
    v = m_utils.callConstFuncForType(ValueType::String, v);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // String -> string
    v = builder->addConstValue("hello world");
    v = m_utils.callConstFuncForType(ValueType::String, v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

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

TEST_F(LLVMCodeBuilderTest, LocalVariables)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerLocalVariable *var1 = builder->createLocalVariable(Compiler::StaticType::Number);
    CompilerLocalVariable *var2 = builder->createLocalVariable(Compiler::StaticType::Number);
    CompilerLocalVariable *var3 = builder->createLocalVariable(Compiler::StaticType::Bool);
    CompilerLocalVariable *var4 = builder->createLocalVariable(Compiler::StaticType::Bool);

    CompilerValue *v = builder->addConstValue(5);
    builder->createLocalVariableWrite(var1, v);

    v = builder->addConstValue(-23.5);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->createLocalVariableWrite(var2, v);

    v = builder->addConstValue(5.2);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->createLocalVariableWrite(var2, v);

    v = builder->addConstValue(false);
    builder->createLocalVariableWrite(var3, v);

    v = builder->addConstValue(true);
    builder->createLocalVariableWrite(var3, v);

    v = builder->addConstValue(false);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->createLocalVariableWrite(var4, v);

    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { builder->addLocalVariableValue(var1) });
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { builder->addLocalVariableValue(var2) });
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { builder->addLocalVariableValue(var3) });
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { builder->addLocalVariableValue(var4) });

    static const std::string expected =
        "5\n"
        "5.2\n"
        "true\n"
        "false\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, WriteVariable)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(5);
    builder->createVariableWrite(globalVar1.get(), v);

    v = builder->addConstValue(-23.5);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->createVariableWrite(globalVar2.get(), v);

    v = builder->addConstValue("test");
    builder->createVariableWrite(globalVar3.get(), v);

    v = builder->addConstValue("abc");
    builder->createVariableWrite(localVar1.get(), v);

    v = builder->addConstValue("hello world");
    v = m_utils.callConstFuncForType(ValueType::String, v);
    builder->createVariableWrite(localVar1.get(), v);

    v = builder->addConstValue(false);
    builder->createVariableWrite(localVar2.get(), v);

    v = builder->addConstValue(true);
    v = m_utils.callConstFuncForType(ValueType::Bool, v);
    builder->createVariableWrite(localVar3.get(), v);

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
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
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    // Number
    CompilerValue *v = builder->addConstValue(true);
    v = builder->createSelect(v, builder->addConstValue(5.8), builder->addConstValue(-17.42), Compiler::StaticType::Number);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addConstValue(false);
    v = builder->createSelect(v, builder->addConstValue(5.8), builder->addConstValue(-17.42), Compiler::StaticType::Number);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Bool
    v = builder->addConstValue(true);
    v = builder->createSelect(v, builder->addConstValue(true), builder->addConstValue(false), Compiler::StaticType::Bool);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue(false);
    v = builder->createSelect(v, builder->addConstValue(true), builder->addConstValue(false), Compiler::StaticType::Bool);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // String
    v = builder->addConstValue(true);
    v = builder->createSelect(v, builder->addConstValue("hello"), builder->addConstValue("world"), Compiler::StaticType::String);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(false);
    v = builder->createSelect(v, builder->addConstValue("hello"), builder->addConstValue("world"), Compiler::StaticType::String);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Different types
    v = builder->addConstValue(true);
    v = builder->createSelect(v, builder->addConstValue("543"), builder->addConstValue("true"), Compiler::StaticType::Number);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addConstValue(false);
    v = builder->createSelect(v, builder->addConstValue("543"), builder->addConstValue("true"), Compiler::StaticType::Number);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addConstValue(true);
    v = builder->createSelect(v, builder->addConstValue(1), builder->addConstValue("false"), Compiler::StaticType::Bool);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    v = builder->addConstValue(false);
    v = builder->createSelect(v, builder->addConstValue(1), builder->addConstValue("false"), Compiler::StaticType::Bool);
    builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

    // Unknown types
    v = builder->addConstValue(true);
    v = builder->createSelect(v, builder->addConstValue("test"), builder->addConstValue(-456.2), Compiler::StaticType::Unknown);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(false);
    v = builder->createSelect(v, builder->addConstValue("abc"), builder->addConstValue(true), Compiler::StaticType::Unknown);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

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
        "0\n"
        "test\n"
        "true\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, ReadVariable)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addVariableValue(globalVar1.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(globalVar2.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(globalVar3.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar1.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar2.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar3.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected;
    expected += globalVar1->value().toString() + '\n';
    expected += globalVar2->value().toString() + '\n';
    expected += globalVar3->value().toString() + '\n';
    expected += localVar1->value().toString() + '\n';
    expected += localVar2->value().toString() + '\n';
    expected += localVar3->value().toString() + '\n';

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, SyncVariablesBeforeCallingFunction)
{
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());

    auto var = std::make_shared<Variable>("", "");
    sprite.addVariable(var);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue("abc");
    builder->createVariableWrite(var.get(), v);

    v = builder->addConstValue(123);
    builder->createVariableWrite(var.get(), v);

    builder->addTargetFunctionCall("test_print_first_local_variable", Compiler::StaticType::Void, {}, {});

    v = builder->addConstValue(456);
    builder->createVariableWrite(var.get(), v);

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);

    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());

    ASSERT_EQ(testing::internal::GetCapturedStdout(), "123\n");
}

TEST_F(LLVMCodeBuilderTest, CastNonRawValueToUnknownType)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto var = std::make_shared<Variable>("", "", 87);
    stage.addVariable(var);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    // Unknown type
    CompilerValue *v = builder->addVariableValue(var.get());
    builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    // Number
    v = builder->addConstValue(23.5);
    builder->createVariableWrite(var.get(), v);
    v = builder->addVariableValue(var.get());
    builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    // String
    v = builder->addConstValue("Hello world");
    builder->createVariableWrite(var.get(), v);
    v = builder->addVariableValue(var.get());
    builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    // Bool
    v = builder->addConstValue(true);
    builder->createVariableWrite(var.get(), v);
    v = builder->addVariableValue(var.get());
    builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    std::string expected;
    expected += var->value().toString() + '\n';
    expected += "23.5\n";
    expected += "Hello world\n";
    expected += "true\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, ClearList)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    builder->createListClear(globalList1.get());
    builder->createListClear(globalList3.get());
    builder->createListClear(localList1.get());
    builder->createListClear(localList2.get());

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
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
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(1);
    builder->createListRemove(globalList.get(), v);

    v = builder->addConstValue(-1);
    builder->createListRemove(globalList.get(), v);

    v = builder->addConstValue(3);
    builder->createListRemove(globalList.get(), v);

    v = builder->addConstValue(3);
    builder->createListRemove(localList.get(), v);

    v = builder->addConstValue(-1);
    builder->createListRemove(localList.get(), v);

    v = builder->addConstValue(4);
    builder->createListRemove(localList.get(), v);

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "13");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor");
}

TEST_F(LLVMCodeBuilderTest, AppendToList)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(1);
    builder->createListAppend(globalList.get(), v);

    v = builder->addConstValue("test");
    builder->createListAppend(globalList.get(), v);

    v = builder->addConstValue(3);
    builder->createListAppend(localList.get(), v);

    builder->createListClear(localList.get());

    v = builder->addConstValue(true);
    builder->createListAppend(localList.get(), v);

    v = builder->addConstValue(false);
    builder->createListAppend(localList.get(), v);

    v = builder->addConstValue("hello world");
    builder->createListAppend(localList.get(), v);

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "1 2 3 1 test");
    ASSERT_EQ(localList->toString(), "true false hello world");
}

TEST_F(LLVMCodeBuilderTest, InsertToList)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v1 = builder->addConstValue(2);
    CompilerValue *v2 = builder->addConstValue(1);
    builder->createListInsert(globalList.get(), v1, v2);

    v1 = builder->addConstValue(3);
    v2 = builder->addConstValue("test");
    builder->createListInsert(globalList.get(), v1, v2);

    v1 = builder->addConstValue(0);
    v2 = builder->addConstValue(3);
    builder->createListInsert(localList.get(), v1, v2);

    builder->createListClear(localList.get());

    v1 = builder->addConstValue(0);
    v2 = builder->addConstValue(true);
    builder->createListInsert(localList.get(), v1, v2);

    v1 = builder->addConstValue(0);
    v2 = builder->addConstValue(false);
    builder->createListInsert(localList.get(), v1, v2);

    v1 = builder->addConstValue(1);
    v2 = builder->addConstValue("hello world");
    builder->createListInsert(localList.get(), v1, v2);

    v1 = builder->addConstValue(3);
    v2 = builder->addConstValue("test");
    builder->createListInsert(localList.get(), v1, v2);

    v1 = builder->addConstValue(-1);
    v2 = builder->addConstValue(123);
    builder->createListInsert(localList.get(), v1, v2);

    v1 = builder->addConstValue(6);
    v2 = builder->addConstValue(123);
    builder->createListInsert(localList.get(), v1, v2);

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "1 2 1 test 3");
    ASSERT_EQ(localList->toString(), "false hello world true test");
}

TEST_F(LLVMCodeBuilderTest, ListReplace)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v1 = builder->addConstValue(2);
    CompilerValue *v2 = builder->addConstValue(1);
    builder->createListReplace(globalList.get(), v1, v2);

    v1 = builder->addConstValue(1);
    v2 = builder->addConstValue("test");
    builder->createListReplace(globalList.get(), v1, v2);

    v1 = builder->addConstValue(0);
    v2 = builder->addConstValue(3);
    builder->createListReplace(localList.get(), v1, v2);

    v1 = builder->addConstValue(2);
    v2 = builder->addConstValue(true);
    builder->createListReplace(localList.get(), v1, v2);

    v1 = builder->addConstValue(3);
    v2 = builder->addConstValue("hello world");
    builder->createListReplace(localList.get(), v1, v2);

    v1 = builder->addConstValue(-1);
    v2 = builder->addConstValue(123);
    builder->createListReplace(localList.get(), v1, v2);

    v1 = builder->addConstValue(5);
    v2 = builder->addConstValue(123);
    builder->createListReplace(localList.get(), v1, v2);

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    code->run(ctx.get());

    ASSERT_EQ(globalList->toString(), "1 test 1");
    ASSERT_EQ(localList->toString(), "3 ipsum true hello world");
}

TEST_F(LLVMCodeBuilderTest, GetListContents)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addListContents(globalList.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListContents(localList.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    static const std::string expected =
        "123\n"
        "Lorem ipsum dolor sit\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "123");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, GetListItem)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList1 = std::make_shared<List>("", "");
    sprite.addList(localList1);

    auto localList2 = std::make_shared<List>("", "");
    sprite.addList(localList2);

    auto localList3 = std::make_shared<List>("", "");
    sprite.addList(localList3);

    globalList->append(1);
    globalList->append(2);
    globalList->append(3);

    localList1->append("Lorem");
    localList1->append("ipsum");
    localList1->append("dolor");
    localList1->append("sit");

    localList2->append(-564.121);
    localList2->append(4257.4);

    localList3->append(true);
    localList3->append(false);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    // Global
    CompilerValue *v = builder->addConstValue(2);
    v = builder->addListItem(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    CompilerValue *v1 = builder->addConstValue(1);
    CompilerValue *v2 = builder->addConstValue("test");
    builder->createListReplace(globalList.get(), v1, v2);

    v = builder->addConstValue(0);
    v = builder->addListItem(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(-1);
    v = builder->addListItem(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(3);
    v = builder->addListItem(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Local 1
    v = builder->addConstValue(0);
    v = builder->addListItem(localList1.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(2);
    v = builder->addListItem(localList1.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(3);
    v = builder->addListItem(localList1.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(-1);
    v = builder->addListItem(localList1.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(4);
    v = builder->addListItem(localList1.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Local 2
    v = builder->addConstValue(-1);
    v = builder->addListItem(localList2.get(), v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addConstValue(2);
    v = builder->addListItem(localList2.get(), v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Local 3
    v = builder->addConstValue(-1);
    v = builder->addListItem(localList3.get(), v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addConstValue(2);
    v = builder->addListItem(localList3.get(), v);
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    static const std::string expected =
        "3\n"
        "1\n"
        "0\n"
        "0\n"
        "Lorem\n"
        "dolor\n"
        "sit\n"
        "0\n"
        "0\n"
        "0\n"
        "0\n"
        "0\n"
        "0\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "1 test 3");
    ASSERT_EQ(localList1->toString(), "Lorem ipsum dolor sit");
    ASSERT_EQ(localList2->toString(), "-564.121 4257.4");
    ASSERT_EQ(localList3->toString(), "true false");
}

TEST_F(LLVMCodeBuilderTest, GetListSize)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addListSize(globalList.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListSize(localList.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    static const std::string expected =
        "3\n"
        "4\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "123");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, GetListItemIndex)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(2);
    v = builder->addListItemIndex(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(1);
    v = builder->addListItemIndex(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(0);
    v = builder->addListItemIndex(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    CompilerValue *v1 = builder->addConstValue(1);
    CompilerValue *v2 = builder->addConstValue("test");
    builder->createListReplace(globalList.get(), v1, v2);

    v = builder->addConstValue(2);
    v = builder->addListItemIndex(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(1);
    v = builder->addListItemIndex(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("test");
    v = builder->addListItemIndex(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("abc");
    v = builder->addListItemIndex(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("doLor");
    v = builder->addListItemIndex(localList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(true);
    v = builder->addListItemIndex(localList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("site");
    v = builder->addListItemIndex(localList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

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

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "1 test 3");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, ListContainsItem)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(2);
    v = builder->addListContains(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(1);
    v = builder->addListContains(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(0);
    v = builder->addListContains(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    CompilerValue *v1 = builder->addConstValue(1);
    CompilerValue *v2 = builder->addConstValue("test");
    builder->createListReplace(globalList.get(), v1, v2);

    v = builder->addConstValue(2);
    v = builder->addListContains(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(1);
    v = builder->addListContains(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("test");
    v = builder->addListContains(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("abc");
    v = builder->addListContains(globalList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("doLor");
    v = builder->addListContains(localList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(true);
    v = builder->addListContains(localList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue("site");
    v = builder->addListContains(localList.get(), v);
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

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

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    ASSERT_EQ(globalList->toString(), "1 test 3");
    ASSERT_EQ(localList->toString(), "Lorem ipsum dolor sit");
}

TEST_F(LLVMCodeBuilderTest, Yield)
{
    LLVMCodeBuilder *builder;

    auto build = [this, &builder]() {
        builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

        CompilerValue *v = builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
        builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        builder->yield();

        v = builder->addConstValue("1");
        v = builder->addTargetFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        CompilerValue *v1 = builder->addConstValue("2");
        CompilerValue *v2 = builder->addConstValue(3);
        builder->addTargetFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String }, { v, v1, v2 });

        v = builder->addConstValue("test");
        v1 = builder->addConstValue("4");
        v2 = builder->addConstValue("5");
        v = builder->addTargetFunctionCall(
            "test_function_3_args_ret",
            Compiler::StaticType::String,
            { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String },
            { v, v1, v2 });
        builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    };

    // Without warp
    builder = m_utils.createBuilder(false);
    build();

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    static const std::string expected1 =
        "no_args\n"
        "no_args_ret\n"
        "1_arg no_args_output\n";

    EXPECT_CALL(m_utils.target(), isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    static const std::string expected2 =
        "1_arg_ret 1\n"
        "3_args 1_arg_output 2 3\n"
        "3_args test 4 5\n"
        "1_arg 3_args_output\n";

    EXPECT_CALL(m_utils.target(), isStage()).Times(4);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected2);
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // Terminate unfinished coroutine
    EXPECT_CALL(m_utils.target(), isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->reset(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    code->kill(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // Reset unfinished coroutine
    EXPECT_CALL(m_utils.target(), isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->reset(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    code->reset(ctx.get());

    EXPECT_CALL(m_utils.target(), isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->reset(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get())); // leave unfinished coroutine

    // With warp
    builder = m_utils.createBuilder(true);
    build();
    code = builder->build();
    ctx = code->createExecutionContext(&thread);

    static const std::string expected =
        "no_args\n"
        "no_args_ret\n"
        "1_arg no_args_output\n"
        "1_arg_ret 1\n"
        "3_args 1_arg_output 2 3\n"
        "3_args test 4 5\n"
        "1_arg 3_args_output\n";

    EXPECT_CALL(m_utils.target(), isStage()).Times(7);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, VariablesAfterSuspend)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "", 87);
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "", "test");
    sprite.addVariable(localVar);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, false);

    CompilerValue *v = builder->addConstValue(12.5);
    builder->createVariableWrite(globalVar.get(), v);

    v = builder->addConstValue(true);
    builder->createVariableWrite(localVar.get(), v);

    builder->yield();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    {
        v = builder->addConstValue(12.5);
        builder->createVariableWrite(globalVar.get(), v);
    }
    builder->endLoop();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(0);
    builder->createVariableWrite(localVar.get(), v);

    builder->beginLoopCondition();
    v = builder->createCmpLT(builder->addVariableValue(localVar.get()), builder->addConstValue(3));
    builder->beginWhileLoop(v);
    builder->endLoop();

    v = builder->addVariableValue(localVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(0);
    builder->createVariableWrite(localVar.get(), v);

    builder->beginLoopCondition();
    v = builder->createCmpEQ(builder->addVariableValue(localVar.get()), builder->addConstValue(2));
    builder->beginRepeatUntilLoop(v);
    builder->endLoop();

    v = builder->addVariableValue(localVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "hello world\n"
        "-4.8\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    code->run(ctx.get());
    ASSERT_FALSE(code->isFinished(ctx.get()));

    globalVar->setValue("hello world");
    localVar->setValue(-4.8);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    globalVar->setValue("test");

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "");
    ASSERT_FALSE(code->isFinished(ctx.get()));

    globalVar->setValue(true);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "true\n");
    ASSERT_FALSE(code->isFinished(ctx.get()));

    localVar->setValue(1);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "");
    ASSERT_FALSE(code->isFinished(ctx.get()));

    localVar->setValue("2");

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "");
    ASSERT_FALSE(code->isFinished(ctx.get()));

    localVar->setValue(3);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "3\n");
    ASSERT_FALSE(code->isFinished(ctx.get()));

    localVar->setValue(1);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "");
    ASSERT_FALSE(code->isFinished(ctx.get()));

    localVar->setValue(2);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "2\n");
}

TEST_F(LLVMCodeBuilderTest, ListsAfterSuspend)
{

    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalList1 = std::make_shared<List>("", "");
    stage.addList(globalList1);

    auto globalList2 = std::make_shared<List>("", "");
    stage.addList(globalList2);

    auto localList1 = std::make_shared<List>("", "");
    sprite.addList(localList1);

    auto localList2 = std::make_shared<List>("", "");
    sprite.addList(localList2);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, false);

    builder->createListClear(globalList1.get());
    builder->createListClear(globalList2.get());
    builder->createListClear(localList1.get());
    builder->createListClear(localList2.get());

    builder->createListAppend(globalList1.get(), builder->addConstValue(1));
    builder->createListAppend(globalList1.get(), builder->addConstValue(2));
    builder->createListAppend(globalList1.get(), builder->addConstValue(3));

    builder->createListAppend(globalList2.get(), builder->addConstValue(1));
    builder->createListAppend(globalList2.get(), builder->addConstValue(2));
    builder->createListAppend(globalList2.get(), builder->addConstValue(3));

    builder->createListReplace(globalList2.get(), builder->addConstValue(1), builder->addConstValue(12.5));

    builder->createListInsert(localList1.get(), builder->addConstValue(0), builder->addConstValue("Lorem"));
    builder->createListInsert(localList1.get(), builder->addConstValue(0), builder->addConstValue("ipsum"));

    builder->createListInsert(localList2.get(), builder->addConstValue(0), builder->addConstValue(true));
    builder->createListInsert(localList2.get(), builder->addConstValue(0), builder->addConstValue(false));

    builder->yield();

    CompilerValue *v = builder->addListItem(globalList1.get(), builder->addConstValue(1));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListItem(globalList1.get(), builder->addConstValue(2));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListItem(globalList2.get(), builder->addConstValue(1));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListItem(globalList2.get(), builder->addConstValue(0));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListItem(localList1.get(), builder->addConstValue(0));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListItem(localList1.get(), builder->addConstValue(1));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListItem(localList2.get(), builder->addConstValue(0));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "2\n"
        "test\n"
        "12.5\n"
        "false\n"
        "ipsum\n"
        "-5.48\n"
        "hello\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
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
    LLVMCodeBuilder *builder = m_utils.createBuilder(true);

    // Without else branch (const condition)
    CompilerValue *v = builder->addConstValue("true");
    builder->beginIfStatement(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endIf();

    v = builder->addConstValue("false");
    builder->beginIfStatement(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endIf();

    // Without else branch (condition returned by function)
    CompilerValue *v1 = builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    CompilerValue *v2 = builder->addConstValue("no_args_output");
    v = builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    builder->beginIfStatement(v);
    v = builder->addConstValue(0);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endIf();

    v1 = builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = builder->addConstValue("");
    v = builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    builder->beginIfStatement(v);
    v = builder->addConstValue(1);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endIf();

    // With else branch (const condition)
    v = builder->addConstValue("true");
    builder->beginIfStatement(v);
    v = builder->addConstValue(2);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->beginElseBranch();
    v = builder->addConstValue(3);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endIf();

    v = builder->addConstValue("false");
    builder->beginIfStatement(v);
    v = builder->addConstValue(4);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->beginElseBranch();
    v = builder->addConstValue(5);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endIf();

    // With else branch (condition returned by function)
    v1 = builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = builder->addConstValue("no_args_output");
    v = builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    builder->beginIfStatement(v);
    v = builder->addConstValue(6);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->beginElseBranch();
    v = builder->addConstValue(7);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endIf();

    v1 = builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = builder->addConstValue("");
    v = builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    builder->beginIfStatement(v);
    v = builder->addConstValue(8);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->beginElseBranch();
    v = builder->addConstValue(9);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endIf();

    // Nested 1
    CompilerValue *str = builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { builder->addConstValue("test") });
    v = builder->addConstValue(true);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue(false);
        builder->beginIfStatement(v);
        {
            v = builder->addConstValue(0);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        builder->beginElseBranch();
        {
            v = builder->addConstValue(1);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

            // str should still be allocated
            builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });

            v = builder->addConstValue(false);
            builder->beginIfStatement(v);
            builder->beginElseBranch();
            {
                v = builder->addConstValue(2);
                builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            }
            builder->endIf();

            // str should still be allocated
            builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });
        }
        builder->endIf();
    }
    builder->beginElseBranch();
    {
        v = builder->addConstValue(true);
        builder->beginIfStatement(v);
        {
            v = builder->addConstValue(3);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        builder->beginElseBranch();
        {
            v = builder->addConstValue(4);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        builder->endIf();
    }
    builder->endIf();

    // str should still be allocated
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });

    // Nested 2
    v = builder->addConstValue(false);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue(false);
        builder->beginIfStatement(v);
        {
            v = builder->addConstValue(5);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        builder->beginElseBranch();
        {
            v = builder->addConstValue(6);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        builder->endIf();
    }
    builder->beginElseBranch();
    {
        str = builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { builder->addConstValue("test") });

        v = builder->addConstValue(true);
        builder->beginIfStatement(v);
        {
            v = builder->addConstValue(7);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        builder->beginElseBranch();
        builder->endIf();

        // str should still be allocated
        builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });
    }
    builder->endIf();

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

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
        "test\n"
        "1_arg 2\n"
        "test\n"
        "test\n"
        "1_arg 7\n"
        "test\n";

    EXPECT_CALL(m_utils.target(), isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, IfStatementVariables)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "", "test");
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "", 87);
    sprite.addVariable(localVar);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(12.5);
    builder->createVariableWrite(globalVar.get(), v);

    v = builder->addConstValue(true);
    builder->createVariableWrite(localVar.get(), v);

    v = builder->addConstValue(true);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue("hello world");
        builder->createVariableWrite(globalVar.get(), v);
    }
    builder->endIf();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(12.5);
    builder->createVariableWrite(globalVar.get(), v);

    v = builder->addConstValue(false);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue("hello world");
        builder->createVariableWrite(globalVar.get(), v);

        v = builder->addConstValue(0);
        builder->createVariableWrite(localVar.get(), v);
    }
    builder->endIf();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(true);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue(true);
        builder->beginIfStatement(v);
        {
            v = builder->addConstValue(true);
            builder->createVariableWrite(globalVar.get(), v);
        }
        builder->endIf();

        v = builder->addConstValue(false);
        builder->beginIfStatement(v);
        {
            v = builder->addConstValue(-8.2);
            builder->createVariableWrite(localVar.get(), v);
        }
        builder->endIf();
    }
    builder->endIf();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "hello world\n"
        "12.5\n"
        "true\n"
        "true\n"
        "true\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, IfStatementLists)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalList1 = std::make_shared<List>("", "");
    stage.addList(globalList1);

    auto globalList2 = std::make_shared<List>("", "");
    stage.addList(globalList2);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, false);

    auto resetLists = [this, builder, globalList1, globalList2, localList]() {
        builder->createListClear(globalList1.get());
        builder->createListClear(globalList2.get());
        builder->createListClear(localList.get());

        builder->createListAppend(globalList1.get(), builder->addConstValue(1));
        builder->createListAppend(globalList1.get(), builder->addConstValue(2));

        builder->createListAppend(globalList2.get(), builder->addConstValue("hello"));
        builder->createListAppend(globalList2.get(), builder->addConstValue("world"));

        builder->createListAppend(localList.get(), builder->addConstValue(false));
        builder->createListAppend(localList.get(), builder->addConstValue(true));
    };

    auto checkLists = [this, builder, globalList1, globalList2, localList]() {
        CompilerValue *v = builder->addListItem(globalList1.get(), builder->addConstValue(0));
        builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = builder->addListItem(globalList2.get(), builder->addConstValue(1));
        builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = builder->addListItem(localList.get(), builder->addConstValue(0));
        builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    };

    // if (true)
    resetLists();

    CompilerValue *v, *v1, *v2;
    v = builder->addConstValue(true);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue("hello world");
        builder->createListAppend(globalList1.get(), v);

        v1 = builder->addConstValue(0);
        v2 = builder->addConstValue(8.5);
        builder->createListInsert(globalList2.get(), v1, v2);

        v1 = builder->addConstValue(1);
        v2 = builder->addConstValue(-4.25);
        builder->createListReplace(localList.get(), v1, v2);
    }
    builder->endIf();

    checkLists();

    // if (false)
    resetLists();

    v = builder->addConstValue(false);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue("hello world");
        builder->createListAppend(globalList1.get(), v);

        v1 = builder->addConstValue(0);
        v2 = builder->addConstValue(8.5);
        builder->createListInsert(globalList2.get(), v1, v2);

        v1 = builder->addConstValue(1);
        v2 = builder->addConstValue(-4.25);
        builder->createListReplace(localList.get(), v1, v2);
    }
    builder->endIf();

    checkLists();

    // if (true) { if (true) { ... }; if (false) { ... } }
    resetLists();

    v = builder->addConstValue(true);
    builder->beginIfStatement(v);
    {
        v = builder->addConstValue(true);
        builder->beginIfStatement(v);
        {
            v = builder->addConstValue("hello world");
            builder->createListAppend(globalList1.get(), v);

            v1 = builder->addConstValue(0);
            v2 = builder->addConstValue(8.5);
            builder->createListInsert(globalList2.get(), v1, v2);
        }
        builder->endIf();

        v = builder->addConstValue(false);
        builder->beginIfStatement(v);
        {
            v1 = builder->addConstValue(1);
            v2 = builder->addConstValue(-4.25);
            builder->createListReplace(localList.get(), v1, v2);
        }
        builder->endIf();
    }
    builder->endIf();

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

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, RepeatLoop)
{
    LLVMCodeBuilder *builder = m_utils.createBuilder(true);

    // Const count
    CompilerValue *v = builder->addConstValue("-5");
    builder->beginRepeatLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    v = builder->addConstValue(0);
    builder->beginRepeatLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    v = builder->addConstValue(3);
    builder->beginRepeatLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    v = builder->addConstValue("2.4");
    builder->beginRepeatLoop(v);
    v = builder->addConstValue(0);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endLoop();

    v = builder->addConstValue("2.5");
    builder->beginRepeatLoop(v);
    v = builder->addConstValue(1);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->endLoop();

    // Count returned by function
    v = builder->addConstValue(2);
    v = m_utils.callConstFuncForType(ValueType::Number, v);
    builder->beginRepeatLoop(v);
    CompilerValue *index = builder->addLoopIndex();
    builder->addTargetFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { index });
    builder->endLoop();

    // Nested
    CompilerValue *str = builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { builder->addConstValue("test") });
    v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    {
        v = builder->addConstValue(2);
        builder->beginRepeatLoop(v);
        {
            v = builder->addConstValue(1);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

            // str should still be allocated
            builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });
        }
        builder->endLoop();

        v = builder->addConstValue(2);
        builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = builder->addConstValue(3);
        builder->beginRepeatLoop(v);
        {
            index = builder->addLoopIndex();
            builder->addTargetFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { index });
        }
        builder->endLoop();
    }
    builder->endLoop();

    // str should still be allocated
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    static const std::string expected =
        "no_args\n"
        "no_args\n"
        "no_args\n"
        "1_arg 0\n"
        "1_arg 0\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "0\n"
        "1\n"
        "1_arg 1\n"
        "test\n"
        "1_arg 1\n"
        "test\n"
        "1_arg 2\n"
        "0\n"
        "1\n"
        "2\n"
        "1_arg 1\n"
        "test\n"
        "1_arg 1\n"
        "test\n"
        "1_arg 2\n"
        "0\n"
        "1\n"
        "2\n"
        "test\n";

    EXPECT_CALL(m_utils.target(), isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    // Yield
    builder = m_utils.createBuilder(false);

    v = builder->addConstValue(3);
    builder->beginRepeatLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    code = builder->build();
    ctx = code->createExecutionContext(&thread);

    static const std::string expected1 = "no_args\n";

    EXPECT_CALL(m_utils.target(), isStage).WillRepeatedly(Return(false));

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
    builder = m_utils.createBuilder(false);

    v = builder->addConstValue(0); // don't yield
    builder->beginRepeatLoop(v);
    builder->endLoop();

    code = builder->build();
    ctx = code->createExecutionContext(&thread);
    code->run(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // Infinite no warp loop
    builder = m_utils.createBuilder(false);

    v = builder->addConstValue("Infinity");
    builder->beginRepeatLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    code = builder->build();
    ctx = code->createExecutionContext(&thread);

    for (int i = 0; i < 10; i++) {
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "no_args\n");
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }
}

TEST_F(LLVMCodeBuilderTest, WhileLoop)
{
    LLVMCodeBuilder *builder = m_utils.createBuilder(true);

    // Const condition
    builder->beginLoopCondition();
    CompilerValue *v = builder->addConstValue("false");
    builder->beginWhileLoop(v);
    builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    builder->beginLoopCondition();
    v = builder->addConstValue(false);
    builder->beginWhileLoop(v);
    builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    // Condition returned by function
    builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    builder->beginLoopCondition();
    CompilerValue *v1 = builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    CompilerValue *v2 = builder->addConstValue(2);
    v = builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    builder->beginWhileLoop(v);
    v = builder->addConstValue(0);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    // Nested
    builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    builder->beginLoopCondition();
    v1 = builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    v2 = builder->addConstValue(3);
    v = builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    builder->beginWhileLoop(v);
    {
        builder->beginLoopCondition();
        v1 = builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
        v2 = builder->addConstValue(3);
        v = builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
        builder->beginWhileLoop(v);
        {
            v = builder->addConstValue(1);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
        }
        builder->endLoop();

        v = builder->addConstValue(2);
        builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        builder->beginLoopCondition();
        v = builder->addConstValue(false);
        builder->beginWhileLoop(v);
        {
            builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
        }
        builder->endLoop();
    }
    builder->endLoop();

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    static const std::string expected =
        "1_arg 0\n"
        "1_arg 0\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 2\n";

    EXPECT_CALL(m_utils.target(), isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    // Yield
    builder = m_utils.createBuilder(false);

    builder->beginLoopCondition();
    v = builder->addConstValue(true);
    builder->beginWhileLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    code = builder->build();
    ctx = code->createExecutionContext(&thread);

    static const std::string expected1 = "no_args\n";

    EXPECT_CALL(m_utils.target(), isStage).WillRepeatedly(Return(false));

    for (int i = 0; i < 10; i++) {
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }
}

TEST_F(LLVMCodeBuilderTest, RepeatUntilLoop)
{
    LLVMCodeBuilder *builder = m_utils.createBuilder(true);

    // Const condition
    builder->beginLoopCondition();
    CompilerValue *v = builder->addConstValue("true");
    builder->beginRepeatUntilLoop(v);
    builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    builder->beginLoopCondition();
    v = builder->addConstValue(true);
    builder->beginRepeatUntilLoop(v);
    builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    // Condition returned by function
    builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    builder->beginLoopCondition();
    CompilerValue *v1 = builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    CompilerValue *v2 = builder->addConstValue(2);
    v = builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    v = builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
    builder->beginRepeatUntilLoop(v);
    v = builder->addConstValue(0);
    builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    // Nested
    builder->addFunctionCall("test_reset_counter", Compiler::StaticType::Void, {}, {});
    builder->beginLoopCondition();
    v1 = builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
    v2 = builder->addConstValue(3);
    v = builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
    v = builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
    builder->beginRepeatUntilLoop(v);
    {
        builder->beginLoopCondition();
        v1 = builder->addFunctionCall("test_get_counter", Compiler::StaticType::Number, {}, {});
        v2 = builder->addConstValue(3);
        v = builder->addFunctionCall("test_lower_than", Compiler::StaticType::Bool, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { v1, v2 });
        v = builder->addFunctionCall("test_not", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
        builder->beginRepeatUntilLoop(v);
        {
            v = builder->addConstValue(1);
            builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
        }
        builder->endLoop();

        v = builder->addConstValue(2);
        builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        builder->beginLoopCondition();
        v = builder->addConstValue(true);
        builder->beginRepeatUntilLoop(v);
        {
            builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
        }
        builder->endLoop();
    }
    builder->endLoop();

    auto code = builder->build();
    Script script(&m_utils.target(), nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_utils.target(), nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    static const std::string expected =
        "1_arg 0\n"
        "1_arg 0\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 1\n"
        "1_arg 2\n";

    EXPECT_CALL(m_utils.target(), isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    // Yield
    builder = m_utils.createBuilder(false);

    builder->beginLoopCondition();
    v = builder->addConstValue(false);
    builder->beginRepeatUntilLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    code = builder->build();
    ctx = code->createExecutionContext(&thread);

    static const std::string expected1 = "no_args\n";

    EXPECT_CALL(m_utils.target(), isStage).WillRepeatedly(Return(false));

    for (int i = 0; i < 10; i++) {
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
        ASSERT_FALSE(code->isFinished(ctx.get()));
    }
}

TEST_F(LLVMCodeBuilderTest, LoopVariables)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "", "test");
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "", 87);
    auto counter1 = std::make_shared<Variable>("", "");
    auto counter2 = std::make_shared<Variable>("", "");
    sprite.addVariable(localVar);
    sprite.addVariable(counter1);
    sprite.addVariable(counter2);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(12.5);
    builder->createVariableWrite(globalVar.get(), v);

    v = builder->addConstValue(true);
    builder->createVariableWrite(localVar.get(), v);

    v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    {
        v = builder->addConstValue("hello world");
        builder->createVariableWrite(globalVar.get(), v);
    }
    builder->endLoop();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(12.5);
    builder->createVariableWrite(globalVar.get(), v);

    v = builder->addConstValue(0);
    builder->beginRepeatLoop(v);
    {
        v = builder->addConstValue("hello world");
        builder->createVariableWrite(globalVar.get(), v);

        v = builder->addConstValue(0);
        builder->createVariableWrite(localVar.get(), v);
    }
    builder->endLoop();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addConstValue(0);
    builder->createVariableWrite(counter1.get(), v);

    builder->beginLoopCondition();
    CompilerValue *v1 = builder->addVariableValue(counter1.get());
    CompilerValue *v2 = builder->addConstValue(5);
    v = builder->createCmpLT(v1, v2);
    builder->beginWhileLoop(v);
    {
        v = builder->addConstValue(0);
        builder->createVariableWrite(counter2.get(), v);

        builder->beginLoopCondition();
        v1 = builder->addVariableValue(counter2.get());
        v2 = builder->addConstValue(3);
        v = builder->createCmpEQ(v1, v2);
        builder->beginRepeatUntilLoop(v);
        {
            v = builder->addConstValue(true);
            builder->createVariableWrite(globalVar.get(), v);

            v1 = builder->addVariableValue(counter2.get());
            v2 = builder->addConstValue(1);
            v = builder->createAdd(v1, v2);
            builder->createVariableWrite(counter2.get(), v);
        }
        builder->endLoop();

        builder->beginLoopCondition();
        v = builder->addConstValue(false);
        builder->beginWhileLoop(v);
        {
            v = builder->addConstValue(-8.2);
            builder->createVariableWrite(localVar.get(), v);
        }
        builder->endLoop();

        v1 = builder->addVariableValue(counter1.get());
        v2 = builder->addConstValue(1);
        v = builder->createAdd(v1, v2);
        builder->createVariableWrite(counter1.get(), v);
    }
    builder->endLoop();

    builder->beginLoopCondition();
    v = builder->addConstValue(true);
    builder->beginRepeatUntilLoop(v);
    {
        v = builder->addConstValue(-8.2);
        builder->createVariableWrite(localVar.get(), v);
    }
    builder->endLoop();

    v = builder->addVariableValue(globalVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addVariableValue(localVar.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "hello world\n"
        "12.5\n"
        "true\n"
        "true\n"
        "true\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, VariableLoopTypeAnalysis)
{
    // This just makes sure the type analyzer is used correctly
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "");
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "");
    sprite.addVariable(localVar);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(5.25);
    builder->createVariableWrite(localVar.get(), v);

    v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    {
        // Type is unknown here because a string is assigned later
        v = builder->addVariableValue(localVar.get());
        builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = builder->addConstValue("test");
        builder->createVariableWrite(localVar.get(), v);
    }
    builder->endLoop();

    std::string expected =
        "5.25\n"
        "0\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, LoopLists)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

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

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    auto resetLists = [this, builder, globalList1, globalList2, localList]() {
        builder->createListClear(globalList1.get());
        builder->createListClear(globalList2.get());
        builder->createListClear(localList.get());

        builder->createListAppend(globalList1.get(), builder->addConstValue(1));
        builder->createListAppend(globalList1.get(), builder->addConstValue(2));

        builder->createListAppend(globalList2.get(), builder->addConstValue("hello"));
        builder->createListAppend(globalList2.get(), builder->addConstValue("world"));

        builder->createListAppend(localList.get(), builder->addConstValue(false));
        builder->createListAppend(localList.get(), builder->addConstValue(true));
    };

    auto checkLists = [this, builder, globalList1, globalList2, localList]() {
        CompilerValue *v = builder->addListItem(globalList1.get(), builder->addConstValue(0));
        builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = builder->addListItem(globalList2.get(), builder->addConstValue(1));
        builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = builder->addListItem(localList.get(), builder->addConstValue(0));
        builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    };

    // repeat (2)
    resetLists();

    CompilerValue *v, *v1, *v2;
    v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    {
        v = builder->addConstValue("hello world");
        builder->createListAppend(globalList1.get(), v);

        v1 = builder->addConstValue(0);
        v2 = builder->addConstValue(8.5);
        builder->createListInsert(globalList2.get(), v1, v2);

        v1 = builder->addConstValue(1);
        v2 = builder->addConstValue(-4.25);
        builder->createListReplace(localList.get(), v1, v2);
    }
    builder->endLoop();

    checkLists();

    // repeat(0)
    resetLists();

    v = builder->addConstValue(0);
    builder->beginRepeatLoop(v);
    {
        v = builder->addConstValue("hello world");
        builder->createListAppend(globalList1.get(), v);

        v1 = builder->addConstValue(0);
        v2 = builder->addConstValue(8.5);
        builder->createListInsert(globalList2.get(), v1, v2);

        v1 = builder->addConstValue(1);
        v2 = builder->addConstValue(-4.25);
        builder->createListReplace(localList.get(), v1, v2);
    }
    builder->endLoop();

    checkLists();

    // while (counter1 < 5) { ... until (counter2 == 3) {... counter2++ }; while (false) { ... }; counter1++ }
    resetLists();

    v = builder->addConstValue(0);
    builder->createVariableWrite(counter1.get(), v);

    builder->beginLoopCondition();
    v1 = builder->addVariableValue(counter1.get());
    v2 = builder->addConstValue(5);
    v = builder->createCmpLT(v1, v2);
    builder->beginWhileLoop(v);
    {
        v = builder->addConstValue(0);
        builder->createVariableWrite(counter2.get(), v);

        builder->beginLoopCondition();
        v1 = builder->addVariableValue(counter2.get());
        v2 = builder->addConstValue(3);
        v = builder->createCmpEQ(v1, v2);
        builder->beginRepeatUntilLoop(v);
        {
            v1 = builder->addConstValue(0);
            v2 = builder->addConstValue(8.5);
            builder->createListInsert(globalList2.get(), v1, v2);

            v1 = builder->addConstValue(1);
            v2 = builder->addConstValue(-4.25);
            builder->createListReplace(localList.get(), v1, v2);

            v1 = builder->addVariableValue(counter2.get());
            v2 = builder->addConstValue(1);
            v = builder->createAdd(v1, v2);
            builder->createVariableWrite(counter2.get(), v);
        }
        builder->endLoop();

        builder->beginLoopCondition();
        v = builder->addConstValue(false);
        builder->beginWhileLoop(v);
        {
            v = builder->addConstValue("hello world");
            builder->createListAppend(globalList1.get(), v);
        }
        builder->endLoop();

        v1 = builder->addVariableValue(counter1.get());
        v2 = builder->addConstValue(1);
        v = builder->createAdd(v1, v2);
        builder->createVariableWrite(counter1.get(), v);
    }
    builder->endLoop();

    checkLists();

    // until (true)
    resetLists();

    builder->beginLoopCondition();
    v = builder->addConstValue(true);
    builder->beginRepeatUntilLoop(v);
    {
        v = builder->addConstValue("hello world");
        builder->createListAppend(globalList1.get(), v);

        v1 = builder->addConstValue(0);
        v2 = builder->addConstValue(8.5);
        builder->createListInsert(globalList2.get(), v1, v2);

        v1 = builder->addConstValue(1);
        v2 = builder->addConstValue(-4.25);
        builder->createListReplace(localList.get(), v1, v2);
    }
    builder->endLoop();

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

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    ;
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, ListLoopTypeAnalysis)
{
    // This just makes sure the type analyzer is used correctly
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_utils.engine());
    EXPECT_CALL(m_utils.engine(), stage()).WillRepeatedly(Return(&stage));

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    builder->createListClear(localList.get());

    CompilerValue *v = builder->addConstValue(5.25);
    builder->createListAppend(localList.get(), v);

    v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    {
        // Type is unknown here because a string is added later
        v = builder->createSub(builder->addListSize(localList.get()), builder->addConstValue(1));
        v = builder->addListItem(localList.get(), v);
        builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = builder->addListItemIndex(localList.get(), builder->addConstValue(5.25));
        builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = builder->addListContains(localList.get(), builder->addConstValue(5.25));
        builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

        v = builder->addConstValue("test");
        builder->createListAppend(localList.get(), v);
    }
    builder->endLoop();

    std::string expected =
        "5.25\n"
        "0\n"
        "1\n"
        "0\n"
        "0\n"
        "1\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, StopNoWarp)
{
    Sprite sprite;
    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, false);

    builder->beginLoopCondition();
    CompilerValue *v = builder->addConstValue(true);
    builder->beginWhileLoop(v);
    builder->createStop();
    builder->endLoop();

    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

    std::string expected = "";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, StopWarp)
{
    Sprite sprite;
    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    CompilerValue *v = builder->addConstValue(true);
    builder->beginIfStatement(v);
    builder->createStop();
    builder->endIf();

    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

    std::string expected = "";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, StopAndReturn)
{
    Sprite sprite;
    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, true);

    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->createStop();

    std::string expected = "no_args\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, MultipleScripts)
{
    Sprite sprite;

    // Script 1
    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, nullptr);

    CompilerValue *v = builder->addConstValue("script1");
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    auto code1 = builder->build();

    // Script 2
    builder = m_utils.createBuilder(&sprite, nullptr);

    v = builder->addConstValue("script2");
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    auto code2 = builder->build();

    Script script1(&sprite, nullptr, nullptr);
    script1.setCode(code1);
    Thread thread1(&sprite, nullptr, &script1);
    auto ctx = code1->createExecutionContext(&thread1);

    testing::internal::CaptureStdout();
    code1->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "script1\n");

    Script script2(&sprite, nullptr, nullptr);
    script2.setCode(code2);
    Thread thread2(&sprite, nullptr, &script2);
    ctx = code2->createExecutionContext(&thread2);

    testing::internal::CaptureStdout();
    code2->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "script2\n");
}

TEST_F(LLVMCodeBuilderTest, Procedures)
{
    Sprite sprite;
    auto var = std::make_shared<Variable>("", "");
    auto list = std::make_shared<List>("", "");
    sprite.addVariable(var);
    sprite.addList(list);

    // Procedure 1
    BlockPrototype prototype1;
    prototype1.setProcCode("procedure 1 %s %s %b");
    prototype1.setArgumentNames({ "any type 1", "any type 2", "bool" });
    prototype1.setArgumentIds({ "a", "b", "c" });
    prototype1.setWarp(false);
    LLVMCodeBuilder *builder = m_utils.createBuilder(&sprite, &prototype1);

    CompilerValue *v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    builder->endLoop();

    builder->createVariableWrite(var.get(), builder->addProcedureArgument("any type 1"));
    builder->createListClear(list.get());
    builder->createListAppend(list.get(), builder->addProcedureArgument("any type 2"));

    v = builder->addVariableValue(var.get());
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addListItem(list.get(), builder->addConstValue(0));
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = builder->addProcedureArgument("bool");
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = builder->addProcedureArgument("invalid");
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    builder->build();

    // Procedure 2
    BlockPrototype prototype2;
    prototype2.setProcCode("procedure 2");
    prototype2.setWarp(true);
    builder = m_utils.createBuilder(&sprite, &prototype2);

    v = builder->addConstValue(2);
    builder->beginRepeatLoop(v);
    builder->createProcedureCall(&prototype1, { builder->addConstValue(-652.3), builder->addConstValue(false), builder->addConstValue(true) });
    builder->endLoop();

    builder->build();

    // Script
    builder = m_utils.createBuilder(&sprite, false);
    builder->createProcedureCall(&prototype1, { builder->addConstValue("test"), builder->addConstValue(true), builder->addConstValue(false) });
    builder->createProcedureCall(&prototype2, {});

    v = builder->addProcedureArgument("test");
    builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected1 = "no_args\n";

    std::string expected2 =
        "test\n"
        "true\n"
        "false\n"
        "0\n"
        "0\n";

    std::string expected3 =
        "no_args\n"
        "no_args\n"
        "-652.3\n"
        "false\n"
        "true\n"
        "1\n"
        "0\n"
        "no_args\n"
        "no_args\n"
        "-652.3\n"
        "false\n"
        "true\n"
        "1\n"
        "0\n"
        "0\n";

    auto code = builder->build();
    Script script(&sprite, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&sprite, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected2 + expected3);
    ASSERT_TRUE(code->isFinished(ctx.get()));
}

TEST_F(LLVMCodeBuilderTest, HatPredicates)
{
    Sprite sprite;

    // Predicate 1
    LLVMCodeBuilder *builder = m_utils.createPredicateBuilder(&sprite);

    builder->addConstValue(true);

    auto code1 = builder->build();

    // Predicate 2
    builder = m_utils.createPredicateBuilder(&sprite);

    CompilerValue *v = builder->addConstValue(false);
    builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });

    auto code2 = builder->build();

    Script script1(&sprite, nullptr, nullptr);
    script1.setCode(code1);
    Thread thread1(&sprite, nullptr, &script1);
    auto ctx = code1->createExecutionContext(&thread1);

    ASSERT_TRUE(code1->runPredicate(ctx.get()));

    Script script2(&sprite, nullptr, nullptr);
    script2.setCode(code2);
    Thread thread2(&sprite, nullptr, &script2);
    ctx = code2->createExecutionContext(&thread2);

    ASSERT_FALSE(code2->runPredicate(ctx.get()));
}

TEST_F(LLVMCodeBuilderTest, Reporters)
{
    Sprite sprite;
    auto var = std::make_shared<Variable>("", "");
    var->setValue("Hello world!");
    sprite.addVariable(var);

    // Reporter 1
    LLVMCodeBuilder *builder = m_utils.createReporterBuilder(&sprite);

    builder->addConstValue(-45.23);

    auto code1 = builder->build();

    // Reporter 2
    builder = m_utils.createReporterBuilder(&sprite);

    CompilerValue *v = builder->addConstValue("test");
    builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });

    auto code2 = builder->build();

    // Reporter 3
    builder = m_utils.createReporterBuilder(&sprite);
    builder->addVariableValue(var.get());

    auto code3 = builder->build();

    // Reporter 4
    builder = m_utils.createReporterBuilder(&sprite);
    int pointee;
    builder->addConstValue(&pointee);

    auto code4 = builder->build();

    // Reporter 5
    builder = m_utils.createReporterBuilder(&sprite);
    v = builder->addConstValue(&pointee);
    builder->addFunctionCall("test_const_pointer", Compiler::StaticType::Pointer, { Compiler::StaticType::Pointer }, { v });

    auto code5 = builder->build();

    auto runReporter = [&sprite](std::shared_ptr<ExecutableCode> code) {
        Script script(&sprite, nullptr, nullptr);
        script.setCode(code);
        Thread thread1(&sprite, nullptr, &script);
        auto ctx = code->createExecutionContext(&thread1);
        return code->runReporter(ctx.get());
    };

    // 1
    ValueData ret = runReporter(code1);
    ASSERT_TRUE(value_isNumber(&ret));
    ASSERT_EQ(value_toDouble(&ret), -45.23);
    value_free(&ret);

    // 2
    ret = runReporter(code2);
    ASSERT_EQ(Value(ret).toString(), "test");
    value_free(&ret);

    // 3
    ret = runReporter(code3);
    var->setValue("abc"); // the string should be copied
    ASSERT_EQ(Value(ret).toString(), "Hello world!");
    value_free(&ret);

    // 4
    ret = runReporter(code4);
    ASSERT_TRUE(value_isPointer(&ret));
    ASSERT_EQ(value_toPointer(&ret), &pointee);
    value_free(&ret);

    // 5
    ret = runReporter(code5);
    ASSERT_TRUE(value_isPointer(&ret));
    ASSERT_EQ(value_toPointer(&ret), &pointee);
    value_free(&ret);
}
