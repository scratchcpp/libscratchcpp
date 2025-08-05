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

#include "testfunctions.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::Eq;

class LLVMCodeBuilderTest : public testing::Test
{
    public:
        void SetUp() override
        {
            test_function(nullptr, nullptr, nullptr, nullptr, nullptr); // force dependency
        }

        void createBuilder(Target *target, BlockPrototype *procedurePrototype, Compiler::CodeType codeType = Compiler::CodeType::Script)
        {
            if (m_contexts.find(target) == m_contexts.cend() || !target)
                m_contexts[target] = std::make_shared<LLVMCompilerContext>(&m_engine, target);

            m_contextList.push_back(m_contexts[target]);
            m_builder = std::make_unique<LLVMCodeBuilder>(m_contexts[target].get(), procedurePrototype, codeType);
        }

        void createBuilder(Target *target, bool warp)
        {
            m_procedurePrototype = std::make_shared<BlockPrototype>("test");
            m_procedurePrototype->setWarp(warp);
            createBuilder(target, m_procedurePrototype.get());
        }

        void createReporterBuilder(Target *target) { createBuilder(target, nullptr, Compiler::CodeType::Reporter); }

        void createPredicateBuilder(Target *target) { createBuilder(target, nullptr, Compiler::CodeType::HatPredicate); }

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

        std::unordered_map<Target *, std::shared_ptr<LLVMCompilerContext>> m_contexts;
        std::vector<std::shared_ptr<LLVMCompilerContext>> m_contextList;
        std::unique_ptr<LLVMCodeBuilder> m_builder;
        std::shared_ptr<BlockPrototype> m_procedurePrototype;
        EngineMock m_engine;
        TargetMock m_target; // NOTE: isStage() is used for call expectations
        RandomGeneratorMock m_rng;
};

TEST_F(LLVMCodeBuilderTest, FunctionCalls)
{
    static const std::vector<bool> warpList = { false, true };

    for (bool warp : warpList) {
        createBuilder(warp);

        m_builder->addFunctionCall("test_empty_function", Compiler::StaticType::Void, {}, {});

        CompilerValue *v = m_builder->addConstValue("test");
        m_builder->addFunctionCallWithCtx("test_ctx_function", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

        v = m_builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
        m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addConstValue("1");
        v = m_builder->addTargetFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        CompilerValue *v1 = m_builder->addConstValue("2");
        CompilerValue *v2 = m_builder->addConstValue("3");
        m_builder
            ->addTargetFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String }, { v, v1, v2 });

        v = m_builder->addConstValue("test");
        v1 = m_builder->addConstValue("4");
        v2 = m_builder->addConstValue("5");
        v = m_builder->addTargetFunctionCall(
            "test_function_3_args_ret",
            Compiler::StaticType::String,
            { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String },
            { v, v1, v2 });
        m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addConstValue(123);
        v = m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { v });
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = m_builder->addConstValue(true);
        v = m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
        m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

        v = m_builder->addConstValue(321.5);
        m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });
        v = m_builder->addFunctionCall("test_const_number", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { v });
        m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

        v = m_builder->addConstValue("test");
        m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });
        v = m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

        v = m_builder->addConstValue(true);
        m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });
        v = m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });
        m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

        auto code = m_builder->build();
        Script script(&m_target, nullptr, nullptr);
        script.setCode(code);
        Thread thread(&m_target, nullptr, &script);
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

        EXPECT_CALL(m_target, isStage()).Times(7);
        testing::internal::CaptureStdout();
        code->run(ctx.get());
        ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
        ASSERT_TRUE(code->isFinished(ctx.get()));
    }
}

TEST_F(LLVMCodeBuilderTest, FunctionCallsWithPointers)
{
    createBuilder(true);

    int var = 12;
    CompilerValue *v = m_builder->addConstValue(&var);
    v = m_builder->addTargetFunctionCall("test_function_1_ptr_arg_ret", Compiler::StaticType::Pointer, { Compiler::StaticType::Pointer }, { v });

    m_builder->addFunctionCall("test_print_pointer", Compiler::StaticType::Void, { Compiler::StaticType::Pointer }, { v });

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

    std::stringstream s;
    s << &m_target;
    std::string ptr = s.str();

    const std::string expected = "1_arg_ret 12\n" + ptr + "\n";

    EXPECT_CALL(m_target, isStage());
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
    ASSERT_TRUE(code->isFinished(ctx.get()));
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

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
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

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

    createBuilder(&sprite, true);

    CompilerLocalVariable *var1 = m_builder->createLocalVariable(Compiler::StaticType::Number);
    CompilerLocalVariable *var2 = m_builder->createLocalVariable(Compiler::StaticType::Number);
    CompilerLocalVariable *var3 = m_builder->createLocalVariable(Compiler::StaticType::Bool);
    CompilerLocalVariable *var4 = m_builder->createLocalVariable(Compiler::StaticType::Bool);

    CompilerValue *v = m_builder->addConstValue(5);
    m_builder->createLocalVariableWrite(var1, v);

    v = m_builder->addConstValue(-23.5);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->createLocalVariableWrite(var2, v);

    v = m_builder->addConstValue(5.2);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->createLocalVariableWrite(var2, v);

    v = m_builder->addConstValue(false);
    m_builder->createLocalVariableWrite(var3, v);

    v = m_builder->addConstValue(true);
    m_builder->createLocalVariableWrite(var3, v);

    v = m_builder->addConstValue(false);
    v = callConstFuncForType(ValueType::Bool, v);
    m_builder->createLocalVariableWrite(var4, v);

    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { m_builder->addLocalVariableValue(var1) });
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { m_builder->addLocalVariableValue(var2) });
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { m_builder->addLocalVariableValue(var3) });
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { m_builder->addLocalVariableValue(var4) });

    static const std::string expected =
        "5\n"
        "5.2\n"
        "true\n"
        "false\n";

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    // Unknown types
    v = m_builder->addConstValue(true);
    v = m_builder->createSelect(v, m_builder->addConstValue("test"), m_builder->addConstValue(-456.2), Compiler::StaticType::Unknown);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(false);
    v = m_builder->createSelect(v, m_builder->addConstValue("abc"), m_builder->addConstValue(true), Compiler::StaticType::Unknown);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);

    auto var = std::make_shared<Variable>("", "");
    sprite.addVariable(var);

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue("abc");
    m_builder->createVariableWrite(var.get(), v);

    v = m_builder->addConstValue(123);
    m_builder->createVariableWrite(var.get(), v);

    m_builder->addTargetFunctionCall("test_print_first_local_variable", Compiler::StaticType::Void, {}, {});

    v = m_builder->addConstValue(456);
    m_builder->createVariableWrite(var.get(), v);

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

    auto var = std::make_shared<Variable>("", "", 87);
    stage.addVariable(var);

    createBuilder(&sprite, true);

    // Unknown type
    CompilerValue *v = m_builder->addVariableValue(var.get());
    m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    // Number
    v = m_builder->addConstValue(23.5);
    m_builder->createVariableWrite(var.get(), v);
    v = m_builder->addVariableValue(var.get());
    m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    // String
    v = m_builder->addConstValue("Hello world");
    m_builder->createVariableWrite(var.get(), v);
    v = m_builder->addVariableValue(var.get());
    m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    // Bool
    v = m_builder->addConstValue(true);
    m_builder->createVariableWrite(var.get(), v);
    v = m_builder->addVariableValue(var.get());
    m_builder->addFunctionCall("test_print_unknown", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { v });

    std::string expected;
    expected += var->value().toString() + '\n';
    expected += "23.5\n";
    expected += "Hello world\n";
    expected += "true\n";

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    v = m_builder->addConstValue(-1);
    m_builder->createListRemove(globalList.get(), v);

    v = m_builder->addConstValue(3);
    m_builder->createListRemove(globalList.get(), v);

    v = m_builder->addConstValue(3);
    m_builder->createListRemove(localList.get(), v);

    v = m_builder->addConstValue(-1);
    m_builder->createListRemove(localList.get(), v);

    v = m_builder->addConstValue(4);
    m_builder->createListRemove(localList.get(), v);

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    v1 = m_builder->addConstValue(3);
    v2 = m_builder->addConstValue("test");
    m_builder->createListInsert(localList.get(), v1, v2);

    v1 = m_builder->addConstValue(-1);
    v2 = m_builder->addConstValue(123);
    m_builder->createListInsert(localList.get(), v1, v2);

    v1 = m_builder->addConstValue(6);
    v2 = m_builder->addConstValue(123);
    m_builder->createListInsert(localList.get(), v1, v2);

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    v1 = m_builder->addConstValue(-1);
    v2 = m_builder->addConstValue(123);
    m_builder->createListReplace(localList.get(), v1, v2);

    v1 = m_builder->addConstValue(5);
    v2 = m_builder->addConstValue(123);
    m_builder->createListReplace(localList.get(), v1, v2);

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    createBuilder(&sprite, true);

    // Global
    CompilerValue *v = m_builder->addConstValue(2);
    v = m_builder->addListItem(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    CompilerValue *v1 = m_builder->addConstValue(1);
    CompilerValue *v2 = m_builder->addConstValue("test");
    m_builder->createListReplace(globalList.get(), v1, v2);

    v = m_builder->addConstValue(0);
    v = m_builder->addListItem(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(-1);
    v = m_builder->addListItem(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(3);
    v = m_builder->addListItem(globalList.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Local 1
    v = m_builder->addConstValue(0);
    v = m_builder->addListItem(localList1.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(2);
    v = m_builder->addListItem(localList1.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(3);
    v = m_builder->addListItem(localList1.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(-1);
    v = m_builder->addListItem(localList1.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(4);
    v = m_builder->addListItem(localList1.get(), v);
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    // Local 2
    v = m_builder->addConstValue(-1);
    v = m_builder->addListItem(localList2.get(), v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addConstValue(2);
    v = m_builder->addListItem(localList2.get(), v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    // Local 3
    v = m_builder->addConstValue(-1);
    v = m_builder->addListItem(localList3.get(), v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addConstValue(2);
    v = m_builder->addListItem(localList3.get(), v);
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    auto build = [this]() {
        m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

        CompilerValue *v = m_builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
        m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        m_builder->yield();

        v = m_builder->addConstValue("1");
        v = m_builder->addTargetFunctionCall("test_function_1_arg_ret", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });
        CompilerValue *v1 = m_builder->addConstValue("2");
        CompilerValue *v2 = m_builder->addConstValue(3);
        m_builder
            ->addTargetFunctionCall("test_function_3_args", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String }, { v, v1, v2 });

        v = m_builder->addConstValue("test");
        v1 = m_builder->addConstValue("4");
        v2 = m_builder->addConstValue("5");
        v = m_builder->addTargetFunctionCall(
            "test_function_3_args_ret",
            Compiler::StaticType::String,
            { Compiler::StaticType::String, Compiler::StaticType::String, Compiler::StaticType::String },
            { v, v1, v2 });
        m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    };

    // Without warp
    createBuilder(false);
    build();

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

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

    // Terminate unfinished coroutine
    EXPECT_CALL(m_target, isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->reset(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    code->kill(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // Reset unfinished coroutine
    EXPECT_CALL(m_target, isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->reset(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get()));

    code->reset(ctx.get());

    EXPECT_CALL(m_target, isStage()).Times(3);
    testing::internal::CaptureStdout();
    code->reset(ctx.get());
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected1);
    ASSERT_FALSE(code->isFinished(ctx.get())); // leave unfinished coroutine

    // With warp
    createBuilder(true);
    build();
    code = m_builder->build();
    ctx = code->createExecutionContext(&thread);

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
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    {
        v = m_builder->addConstValue(12.5);
        m_builder->createVariableWrite(globalVar.get(), v);
    }
    m_builder->endLoop();

    v = m_builder->addVariableValue(globalVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(0);
    m_builder->createVariableWrite(localVar.get(), v);

    m_builder->beginLoopCondition();
    v = m_builder->createCmpLT(m_builder->addVariableValue(localVar.get()), m_builder->addConstValue(3));
    m_builder->beginWhileLoop(v);
    m_builder->endLoop();

    v = m_builder->addVariableValue(localVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addConstValue(0);
    m_builder->createVariableWrite(localVar.get(), v);

    m_builder->beginLoopCondition();
    v = m_builder->createCmpEQ(m_builder->addVariableValue(localVar.get()), m_builder->addConstValue(2));
    m_builder->beginRepeatUntilLoop(v);
    m_builder->endLoop();

    v = m_builder->addVariableValue(localVar.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    std::string expected =
        "hello world\n"
        "-4.8\n";

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    createBuilder(true);

    // Without else branch (const condition)
    CompilerValue *v = m_builder->addConstValue("true");
    m_builder->beginIfStatement(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endIf();

    v = m_builder->addConstValue("false");
    m_builder->beginIfStatement(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endIf();

    // Without else branch (condition returned by function)
    CompilerValue *v1 = m_builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    CompilerValue *v2 = m_builder->addConstValue("no_args_output");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(0);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    v1 = m_builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = m_builder->addConstValue("");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(1);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    // With else branch (const condition)
    v = m_builder->addConstValue("true");
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(2);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(3);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    v = m_builder->addConstValue("false");
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(4);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(5);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    // With else branch (condition returned by function)
    v1 = m_builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = m_builder->addConstValue("no_args_output");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(6);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(7);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    v1 = m_builder->addTargetFunctionCall("test_function_no_args_ret", Compiler::StaticType::String, {}, {});
    v2 = m_builder->addConstValue("");
    v = m_builder->addFunctionCall("test_equals", Compiler::StaticType::Bool, { Compiler::StaticType::String, Compiler::StaticType::String }, { v1, v2 });
    m_builder->beginIfStatement(v);
    v = m_builder->addConstValue(8);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->beginElseBranch();
    v = m_builder->addConstValue(9);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endIf();

    // Nested 1
    CompilerValue *str = m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { m_builder->addConstValue("test") });
    v = m_builder->addConstValue(true);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue(false);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(0);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->beginElseBranch();
        {
            v = m_builder->addConstValue(1);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

            // str should still be allocated
            m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });

            v = m_builder->addConstValue(false);
            m_builder->beginIfStatement(v);
            m_builder->beginElseBranch();
            {
                v = m_builder->addConstValue(2);
                m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            }
            m_builder->endIf();

            // str should still be allocated
            m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });
        }
        m_builder->endIf();
    }
    m_builder->beginElseBranch();
    {
        v = m_builder->addConstValue(true);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(3);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->beginElseBranch();
        {
            v = m_builder->addConstValue(4);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->endIf();
    }
    m_builder->endIf();

    // str should still be allocated
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });

    // Nested 2
    v = m_builder->addConstValue(false);
    m_builder->beginIfStatement(v);
    {
        v = m_builder->addConstValue(false);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(5);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->beginElseBranch();
        {
            v = m_builder->addConstValue(6);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->endIf();
    }
    m_builder->beginElseBranch();
    {
        str = m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { m_builder->addConstValue("test") });

        v = m_builder->addConstValue(true);
        m_builder->beginIfStatement(v);
        {
            v = m_builder->addConstValue(7);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
        }
        m_builder->beginElseBranch();
        m_builder->endIf();

        // str should still be allocated
        m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });
    }
    m_builder->endIf();

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
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

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(LLVMCodeBuilderTest, IfStatementVariables)
{
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    createBuilder(true);

    // Const count
    CompilerValue *v = m_builder->addConstValue("-5");
    m_builder->beginRepeatLoop(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    v = m_builder->addConstValue(0);
    m_builder->beginRepeatLoop(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    v = m_builder->addConstValue(3);
    m_builder->beginRepeatLoop(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    v = m_builder->addConstValue("2.4");
    m_builder->beginRepeatLoop(v);
    v = m_builder->addConstValue(0);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endLoop();

    v = m_builder->addConstValue("2.5");
    m_builder->beginRepeatLoop(v);
    v = m_builder->addConstValue(1);
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->endLoop();

    // Count returned by function
    v = m_builder->addConstValue(2);
    v = callConstFuncForType(ValueType::Number, v);
    m_builder->beginRepeatLoop(v);
    CompilerValue *index = m_builder->addLoopIndex();
    m_builder->addTargetFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { index });
    m_builder->endLoop();

    // Nested
    CompilerValue *str = m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { m_builder->addConstValue("test") });
    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    {
        v = m_builder->addConstValue(2);
        m_builder->beginRepeatLoop(v);
        {
            v = m_builder->addConstValue(1);
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

            // str should still be allocated
            m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });
        }
        m_builder->endLoop();

        v = m_builder->addConstValue(2);
        m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        v = m_builder->addConstValue(3);
        m_builder->beginRepeatLoop(v);
        {
            index = m_builder->addLoopIndex();
            m_builder->addTargetFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { index });
        }
        m_builder->endLoop();
    }
    m_builder->endLoop();

    // str should still be allocated
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { str });

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
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

    EXPECT_CALL(m_target, isStage).WillRepeatedly(Return(false));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);

    // Yield
    createBuilder(false);

    v = m_builder->addConstValue(3);
    m_builder->beginRepeatLoop(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    code = m_builder->build();
    ctx = code->createExecutionContext(&thread);

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

    code = m_builder->build();
    ctx = code->createExecutionContext(&thread);
    code->run(ctx.get());
    ASSERT_TRUE(code->isFinished(ctx.get()));

    // Infinite no warp loop
    createBuilder(false);

    v = m_builder->addConstValue("Infinity");
    m_builder->beginRepeatLoop(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    code = m_builder->build();
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
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
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
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
        }
        m_builder->endLoop();

        v = m_builder->addConstValue(2);
        m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        m_builder->beginLoopCondition();
        v = m_builder->addConstValue(false);
        m_builder->beginWhileLoop(v);
        {
            m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
        }
        m_builder->endLoop();
    }
    m_builder->endLoop();

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

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
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    code = m_builder->build();
    ctx = code->createExecutionContext(&thread);

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
    m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
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
            m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
            m_builder->addFunctionCall("test_increment_counter", Compiler::StaticType::Void, {}, {});
        }
        m_builder->endLoop();

        v = m_builder->addConstValue(2);
        m_builder->addTargetFunctionCall("test_function_1_arg", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

        m_builder->beginLoopCondition();
        v = m_builder->addConstValue(true);
        m_builder->beginRepeatUntilLoop(v);
        {
            m_builder->addFunctionCall("test_unreachable", Compiler::StaticType::Void, {}, {});
        }
        m_builder->endLoop();
    }
    m_builder->endLoop();

    auto code = m_builder->build();
    Script script(&m_target, nullptr, nullptr);
    script.setCode(code);
    Thread thread(&m_target, nullptr, &script);
    auto ctx = code->createExecutionContext(&thread);

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
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    code = m_builder->build();
    ctx = code->createExecutionContext(&thread);

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
    Stage stage;
    Sprite sprite;
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

    auto globalVar = std::make_shared<Variable>("", "");
    stage.addVariable(globalVar);

    auto localVar = std::make_shared<Variable>("", "");
    sprite.addVariable(localVar);

    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(5.25);
    m_builder->createVariableWrite(localVar.get(), v);

    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    {
        // Type is unknown here because a string is assigned later
        v = m_builder->addVariableValue(localVar.get());
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = m_builder->addConstValue("test");
        m_builder->createVariableWrite(localVar.get(), v);
    }
    m_builder->endLoop();

    std::string expected =
        "5.25\n"
        "0\n";

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

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

    auto code = m_builder->build();
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
    sprite.setEngine(&m_engine);
    EXPECT_CALL(m_engine, stage()).WillRepeatedly(Return(&stage));

    auto globalList = std::make_shared<List>("", "");
    stage.addList(globalList);

    auto localList = std::make_shared<List>("", "");
    sprite.addList(localList);

    createBuilder(&sprite, true);

    m_builder->createListClear(localList.get());

    CompilerValue *v = m_builder->addConstValue(5.25);
    m_builder->createListAppend(localList.get(), v);

    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    {
        // Type is unknown here because a string is added later
        v = m_builder->createSub(m_builder->addListSize(localList.get()), m_builder->addConstValue(1));
        v = m_builder->addListItem(localList.get(), v);
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = m_builder->addListItemIndex(localList.get(), m_builder->addConstValue(5.25));
        m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

        v = m_builder->addListContains(localList.get(), m_builder->addConstValue(5.25));
        m_builder->addFunctionCall("test_print_bool", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { v });

        v = m_builder->addConstValue("test");
        m_builder->createListAppend(localList.get(), v);
    }
    m_builder->endLoop();

    std::string expected =
        "5.25\n"
        "0\n"
        "1\n"
        "0\n"
        "0\n"
        "1\n";

    auto code = m_builder->build();
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
    createBuilder(&sprite, false);

    m_builder->beginLoopCondition();
    CompilerValue *v = m_builder->addConstValue(true);
    m_builder->beginWhileLoop(v);
    m_builder->createStop();
    m_builder->endLoop();

    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

    std::string expected = "";

    auto code = m_builder->build();
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
    createBuilder(&sprite, true);

    CompilerValue *v = m_builder->addConstValue(true);
    m_builder->beginIfStatement(v);
    m_builder->createStop();
    m_builder->endIf();

    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});

    std::string expected = "";

    auto code = m_builder->build();
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
    createBuilder(&sprite, true);

    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->createStop();

    std::string expected = "no_args\n";

    auto code = m_builder->build();
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
    createBuilder(&sprite, nullptr);

    CompilerValue *v = m_builder->addConstValue("script1");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    auto code1 = m_builder->build();

    // Script 2
    createBuilder(&sprite, nullptr);

    v = m_builder->addConstValue("script2");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    auto code2 = m_builder->build();

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
    createBuilder(&sprite, &prototype1);

    CompilerValue *v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    m_builder->addTargetFunctionCall("test_function_no_args", Compiler::StaticType::Void, {}, {});
    m_builder->endLoop();

    m_builder->createVariableWrite(var.get(), m_builder->addProcedureArgument("any type 1"));
    m_builder->createListClear(list.get());
    m_builder->createListAppend(list.get(), m_builder->addProcedureArgument("any type 2"));

    v = m_builder->addVariableValue(var.get());
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addListItem(list.get(), m_builder->addConstValue(0));
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    v = m_builder->addProcedureArgument("bool");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });
    m_builder->addFunctionCall("test_print_number", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { v });

    v = m_builder->addProcedureArgument("invalid");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

    m_builder->build();

    // Procedure 2
    BlockPrototype prototype2;
    prototype2.setProcCode("procedure 2");
    prototype2.setWarp(true);
    createBuilder(&sprite, &prototype2);

    v = m_builder->addConstValue(2);
    m_builder->beginRepeatLoop(v);
    m_builder->createProcedureCall(&prototype1, { m_builder->addConstValue(-652.3), m_builder->addConstValue(false), m_builder->addConstValue(true) });
    m_builder->endLoop();

    m_builder->build();

    // Script
    createBuilder(&sprite, false);
    m_builder->createProcedureCall(&prototype1, { m_builder->addConstValue("test"), m_builder->addConstValue(true), m_builder->addConstValue(false) });
    m_builder->createProcedureCall(&prototype2, {});

    v = m_builder->addProcedureArgument("test");
    m_builder->addFunctionCall("test_print_string", Compiler::StaticType::Void, { Compiler::StaticType::String }, { v });

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

    auto code = m_builder->build();
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
    createPredicateBuilder(&sprite);

    m_builder->addConstValue(true);

    auto code1 = m_builder->build();

    // Predicate 2
    createPredicateBuilder(&sprite);

    CompilerValue *v = m_builder->addConstValue(false);
    m_builder->addFunctionCall("test_const_bool", Compiler::StaticType::Bool, { Compiler::StaticType::Bool }, { v });

    auto code2 = m_builder->build();

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
    createReporterBuilder(&sprite);

    m_builder->addConstValue(-45.23);

    auto code1 = m_builder->build();

    // Reporter 2
    createReporterBuilder(&sprite);

    CompilerValue *v = m_builder->addConstValue("test");
    m_builder->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { v });

    auto code2 = m_builder->build();

    // Reporter 3
    createReporterBuilder(&sprite);
    m_builder->addVariableValue(var.get());

    auto code3 = m_builder->build();

    // Reporter 4
    createReporterBuilder(&sprite);
    int pointee;
    m_builder->addConstValue(&pointee);

    auto code4 = m_builder->build();

    // Reporter 5
    createReporterBuilder(&sprite);
    v = m_builder->addConstValue(&pointee);
    m_builder->addFunctionCall("test_const_pointer", Compiler::StaticType::Pointer, { Compiler::StaticType::Pointer }, { v });

    auto code5 = m_builder->build();

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
