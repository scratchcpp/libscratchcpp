#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/list.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <enginemock.h>
#include <randomgeneratormock.h>

#include "../common.h"
#include "blocks/operatorblocks.h"
#include "util.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;

class OperatorBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<OperatorBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            registerBlocks(m_engine, m_extension.get());
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
        RandomGeneratorMock m_rng;
};

TEST_F(OperatorBlocksTest, Add)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_add");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 1);
    ASSERT_EQ(Value(values[0]), 8.2);
}

TEST_F(OperatorBlocksTest, Subtract)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_subtract");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 1);
    ASSERT_EQ(Value(values[0]), 3.2);
}

TEST_F(OperatorBlocksTest, Multiply)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_multiply");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 1);
    ASSERT_EQ(Value(values[0]), 14.25);
}

TEST_F(OperatorBlocksTest, Divide)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_divide");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 1);
    ASSERT_EQ(std::round(value_toDouble(&values[0]) * 100) / 100, 2.28);
}

TEST_F(OperatorBlocksTest, Random)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    auto addRandomTest = [&builder](const Value &from, const Value &to) {
        auto block = std::make_shared<Block>("", "operator_random");
        auto input = std::make_shared<Input>("FROM", Input::Type::Shadow);
        input->setPrimaryValue(from);
        block->addInput(input);
        input = std::make_shared<Input>("TO", Input::Type::Shadow);
        input->setPrimaryValue(to);
        block->addInput(input);

        builder.addBlock("test_print");
        builder.addObscuredInput("STRING", block);
        return builder.currentBlock();
    };

    auto block = addRandomTest(-45, 12);
    addRandomTest(12, 6.05);
    addRandomTest(-78.686, -45);
    addRandomTest(6.05, -78.686);

    builder.build();

    Compiler compiler(&m_engineMock, target.get());
    auto code = compiler.compile(block);
    Script script(target.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(target.get(), &m_engineMock, &script);
    auto ctx = code->createExecutionContext(&thread);
    ctx->setRng(&m_rng);

    static const std::string expected =
        "-18\n"
        "3.486789\n"
        "-59.468873\n"
        "-28.648764\n";

    EXPECT_CALL(m_rng, randint(-45, 12)).WillOnce(Return(-18));
    EXPECT_CALL(m_rng, randintDouble(12, 6.05)).WillOnce(Return(3.486789));
    EXPECT_CALL(m_rng, randintDouble(-78.686, -45)).WillOnce(Return(-59.468873));
    EXPECT_CALL(m_rng, randintDouble(6.05, -78.686)).WillOnce(Return(-28.648764));
    testing::internal::CaptureStdout();
    code->run(ctx.get());
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expected);
}

TEST_F(OperatorBlocksTest, Lt)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_lt");
    builder.addValueInput("OPERAND1", 5.4645);
    builder.addValueInput("OPERAND2", 12.486);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_lt");
    builder.addValueInput("OPERAND1", 153.25);
    builder.addValueInput("OPERAND2", 96.5);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_lt");
    builder.addValueInput("OPERAND1", 2.8465);
    builder.addValueInput("OPERAND2", 2.8465);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 3);
    ASSERT_EQ(Value(values[0]), true);
    ASSERT_EQ(Value(values[1]), false);
    ASSERT_EQ(Value(values[2]), false);
}

TEST_F(OperatorBlocksTest, Equals)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_equals");
    builder.addValueInput("OPERAND1", 5.4645);
    builder.addValueInput("OPERAND2", 12.486);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_equals");
    builder.addValueInput("OPERAND1", 153.25);
    builder.addValueInput("OPERAND2", 96.5);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_equals");
    builder.addValueInput("OPERAND1", 2.8465);
    builder.addValueInput("OPERAND2", 2.8465);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 3);
    ASSERT_EQ(Value(values[0]), false);
    ASSERT_EQ(Value(values[1]), false);
    ASSERT_EQ(Value(values[2]), true);
}

TEST_F(OperatorBlocksTest, Gt)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_gt");
    builder.addValueInput("OPERAND1", 5.4645);
    builder.addValueInput("OPERAND2", 12.486);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_gt");
    builder.addValueInput("OPERAND1", 153.25);
    builder.addValueInput("OPERAND2", 96.5);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_gt");
    builder.addValueInput("OPERAND1", 2.8465);
    builder.addValueInput("OPERAND2", 2.8465);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 3);
    ASSERT_EQ(Value(values[0]), false);
    ASSERT_EQ(Value(values[1]), true);
    ASSERT_EQ(Value(values[2]), false);
}

TEST_F(OperatorBlocksTest, And)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_and");
    builder.addValueInput("OPERAND1", false);
    builder.addValueInput("OPERAND2", false);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_and");
    builder.addValueInput("OPERAND1", true);
    builder.addValueInput("OPERAND2", false);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_and");
    builder.addValueInput("OPERAND1", false);
    builder.addValueInput("OPERAND2", true);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_and");
    builder.addValueInput("OPERAND1", true);
    builder.addValueInput("OPERAND2", true);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 4);
    ASSERT_EQ(Value(values[0]), false);
    ASSERT_EQ(Value(values[1]), false);
    ASSERT_EQ(Value(values[2]), false);
    ASSERT_EQ(Value(values[3]), true);
}

TEST_F(OperatorBlocksTest, Or)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_or");
    builder.addValueInput("OPERAND1", false);
    builder.addValueInput("OPERAND2", false);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_or");
    builder.addValueInput("OPERAND1", true);
    builder.addValueInput("OPERAND2", false);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_or");
    builder.addValueInput("OPERAND1", false);
    builder.addValueInput("OPERAND2", true);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_or");
    builder.addValueInput("OPERAND1", true);
    builder.addValueInput("OPERAND2", true);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 4);
    ASSERT_EQ(Value(values[0]), false);
    ASSERT_EQ(Value(values[1]), true);
    ASSERT_EQ(Value(values[2]), true);
    ASSERT_EQ(Value(values[3]), true);
}

TEST_F(OperatorBlocksTest, Not)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_not");
    builder.addValueInput("OPERAND", false);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_not");
    builder.addValueInput("OPERAND", true);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 2);
    ASSERT_EQ(Value(values[0]), true);
    ASSERT_EQ(Value(values[1]), false);
}

TEST_F(OperatorBlocksTest, Join)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_join");
    builder.addValueInput("STRING1", "abc");
    builder.addValueInput("STRING2", "def");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_join");
    builder.addValueInput("STRING1", "Hello ");
    builder.addValueInput("STRING2", "world");
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 2);
    ASSERT_EQ(Value(values[0]).toString(), "abcdef");
    ASSERT_EQ(Value(values[1]).toString(), "Hello world");
}

TEST_F(OperatorBlocksTest, LetterOf)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_letter_of");
    builder.addValueInput("LETTER", 2);
    builder.addValueInput("STRING", "abc");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_letter_of");
    builder.addValueInput("LETTER", 7);
    builder.addValueInput("STRING", "Hello world");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_letter_of");
    builder.addValueInput("LETTER", 0);
    builder.addValueInput("STRING", "Hello world");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_letter_of");
    builder.addValueInput("LETTER", 12);
    builder.addValueInput("STRING", "Hello world");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_letter_of");
    builder.addValueInput("LETTER", 1);
    builder.addValueInput("STRING", "Ábč");
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 5);
    ASSERT_EQ(Value(values[0]).toString(), "b");
    ASSERT_EQ(Value(values[1]).toString(), "w");
    ASSERT_EQ(Value(values[2]).toString(), "");
    ASSERT_EQ(Value(values[3]).toString(), "");
    ASSERT_EQ(Value(values[4]).toString(), "Á");
}

TEST_F(OperatorBlocksTest, Length)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_length");
    builder.addValueInput("STRING", "abc");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_length");
    builder.addValueInput("STRING", "Hello world");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_length");
    builder.addValueInput("STRING", "dOádčĐaší");
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 3);
    ASSERT_EQ(Value(values[0]), 3);
    ASSERT_EQ(Value(values[1]), 11);
    ASSERT_EQ(Value(values[2]), 9);
}

TEST_F(OperatorBlocksTest, Contains)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "abc");
    builder.addValueInput("STRING2", "a");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "abc");
    builder.addValueInput("STRING2", "e");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "abc");
    builder.addValueInput("STRING2", "C");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "Hello world");
    builder.addValueInput("STRING2", "ello");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "Hello world");
    builder.addValueInput("STRING2", "olld");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "ábČ");
    builder.addValueInput("STRING2", "á");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "ábČ");
    builder.addValueInput("STRING2", "bČ");
    builder.captureBlockReturnValue();

    builder.addBlock("operator_contains");
    builder.addValueInput("STRING1", "ábČ");
    builder.addValueInput("STRING2", "ďá");
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 8);
    ASSERT_EQ(Value(values[0]), true);
    ASSERT_EQ(Value(values[1]), false);
    ASSERT_EQ(Value(values[2]), true);
    ASSERT_EQ(Value(values[3]), true);
    ASSERT_EQ(Value(values[4]), false);
    ASSERT_EQ(Value(values[5]), true);
    ASSERT_EQ(Value(values[6]), true);
    ASSERT_EQ(Value(values[7]), false);
}

TEST_F(OperatorBlocksTest, Mod)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_mod");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mod");
    builder.addValueInput("NUM1", -5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 2);
    ASSERT_EQ(std::round(value_toDouble(&values[0]) * 100) / 100, 0.7);
    ASSERT_EQ(std::round(value_toDouble(&values[1]) * 100) / 100, 1.8);
}

TEST_F(OperatorBlocksTest, Round)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_round");
    builder.addValueInput("NUM", 5.7);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_round");
    builder.addValueInput("NUM", 2.3);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 2);
    ASSERT_EQ(Value(values[0]), 6);
    ASSERT_EQ(Value(values[1]), 2);
}

TEST_F(OperatorBlocksTest, MathOp)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    // abs
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "abs");
    builder.addValueInput("NUM", 5.7);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "abs");
    builder.addValueInput("NUM", -5.7);
    builder.captureBlockReturnValue();

    // floor
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "floor");
    builder.addValueInput("NUM", 3.2);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "floor");
    builder.addValueInput("NUM", 5.7);
    builder.captureBlockReturnValue();

    // ceiling
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "ceiling");
    builder.addValueInput("NUM", 3.2);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "ceiling");
    builder.addValueInput("NUM", 5.7);
    builder.captureBlockReturnValue();

    // sqrt
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "sqrt");
    builder.addValueInput("NUM", 16);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "sqrt");
    builder.addValueInput("NUM", 2);
    builder.captureBlockReturnValue();

    // sin
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "sin");
    builder.addValueInput("NUM", 90);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "sin");
    builder.addValueInput("NUM", 30);
    builder.captureBlockReturnValue();

    // cos
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "cos");
    builder.addValueInput("NUM", 0);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "cos");
    builder.addValueInput("NUM", 60);
    builder.captureBlockReturnValue();

    // tan
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "tan");
    builder.addValueInput("NUM", 30);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "tan");
    builder.addValueInput("NUM", 45);
    builder.captureBlockReturnValue();

    // asin
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "asin");
    builder.addValueInput("NUM", 1);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "asin");
    builder.addValueInput("NUM", 0.5);
    builder.captureBlockReturnValue();

    // acos
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "acos");
    builder.addValueInput("NUM", 1);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "acos");
    builder.addValueInput("NUM", 0.5);
    builder.captureBlockReturnValue();

    // atan
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "atan");
    builder.addValueInput("NUM", 0.5);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "atan");
    builder.addValueInput("NUM", 1);
    builder.captureBlockReturnValue();

    // ln
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "ln");
    builder.addValueInput("NUM", 1);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "ln");
    builder.addValueInput("NUM", 10);
    builder.captureBlockReturnValue();

    // log
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "log");
    builder.addValueInput("NUM", 1);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "log");
    builder.addValueInput("NUM", 100);
    builder.captureBlockReturnValue();

    // e ^
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "e ^");
    builder.addValueInput("NUM", 1);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "e ^");
    builder.addValueInput("NUM", 8.2);
    builder.captureBlockReturnValue();

    // 10 ^
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "10 ^");
    builder.addValueInput("NUM", 1);
    builder.captureBlockReturnValue();

    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "10 ^");
    builder.addValueInput("NUM", 8.2);
    builder.captureBlockReturnValue();

    // invalid
    builder.addBlock("operator_mathop");
    builder.addDropdownField("OPERATOR", "invalid");
    builder.addValueInput("NUM", -5.54);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 29);
    ASSERT_EQ(std::round(value_toDouble(&values[0]) * 100) / 100, 5.7);
    ASSERT_EQ(std::round(value_toDouble(&values[1]) * 100) / 100, 5.7);
    ASSERT_EQ(std::round(value_toDouble(&values[2]) * 100) / 100, 3);
    ASSERT_EQ(std::round(value_toDouble(&values[3]) * 100) / 100, 5);
    ASSERT_EQ(std::round(value_toDouble(&values[4]) * 100) / 100, 4);
    ASSERT_EQ(std::round(value_toDouble(&values[5]) * 100) / 100, 6);
    ASSERT_EQ(std::round(value_toDouble(&values[6]) * 100) / 100, 4);
    ASSERT_EQ(std::round(value_toDouble(&values[7]) * 100) / 100, 1.41);
    ASSERT_EQ(std::round(value_toDouble(&values[8]) * 100) / 100, 1);
    ASSERT_EQ(std::round(value_toDouble(&values[9]) * 100) / 100, 0.5);
    ASSERT_EQ(std::round(value_toDouble(&values[10]) * 100) / 100, 1);
    ASSERT_EQ(std::round(value_toDouble(&values[11]) * 100) / 100, 0.5);
    ASSERT_EQ(std::round(value_toDouble(&values[12]) * 100) / 100, 0.58);
    ASSERT_EQ(std::round(value_toDouble(&values[13]) * 100) / 100, 1);
    ASSERT_EQ(std::round(value_toDouble(&values[14]) * 100) / 100, 90);
    ASSERT_EQ(std::round(value_toDouble(&values[15]) * 100) / 100, 30);
    ASSERT_EQ(std::round(value_toDouble(&values[16]) * 100) / 100, 0);
    ASSERT_EQ(std::round(value_toDouble(&values[17]) * 100) / 100, 60);
    ASSERT_EQ(std::round(value_toDouble(&values[18]) * 100) / 100, 26.57);
    ASSERT_EQ(std::round(value_toDouble(&values[19]) * 100) / 100, 45);
    ASSERT_EQ(std::round(value_toDouble(&values[20]) * 100) / 100, 0);
    ASSERT_EQ(std::round(value_toDouble(&values[21]) * 100) / 100, 2.3);
    ASSERT_EQ(std::round(value_toDouble(&values[22]) * 100) / 100, 0);
    ASSERT_EQ(std::round(value_toDouble(&values[23]) * 100) / 100, 2);
    ASSERT_EQ(std::round(value_toDouble(&values[24]) * 100) / 100, 2.72);
    ASSERT_EQ(std::round(value_toDouble(&values[25]) * 100) / 100, 3640.95);
    ASSERT_EQ(std::round(value_toDouble(&values[26]) * 100) / 100, 10);
    ASSERT_EQ(std::round(value_toDouble(&values[27]) * 100) / 100, 158489319.25);
    ASSERT_EQ(std::round(value_toDouble(&values[28]) * 100) / 100, 0);
}
