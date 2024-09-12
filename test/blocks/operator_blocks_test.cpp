#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/operatorblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class OperatorBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<OperatorBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For any operator block
        std::shared_ptr<Block> createOperatorBlock(const std::string &id, const std::string &opcode) const { return std::make_shared<Block>(id, opcode); }

        void addInput(std::shared_ptr<Block> block, const std::string &name, OperatorBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, OperatorBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        void addNullInput(std::shared_ptr<Block> block, const std::string &name, OperatorBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);
        }

        // For the mathop block
        void addMathOpField(std::shared_ptr<Block> block, const std::string &operatorStr, OperatorBlocks::FieldValues operatorId)
        {
            auto field = std::make_shared<Field>("OPERATOR", operatorStr);
            field->setFieldId(OperatorBlocks::OPERATOR);
            field->setSpecialValueId(operatorId);
            block->addField(field);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(OperatorBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Operators");
}

TEST_F(OperatorBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(OperatorBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_add", &OperatorBlocks::compileAdd)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_subtract", &OperatorBlocks::compileSubtract)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_multiply", &OperatorBlocks::compileMultiply)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_divide", &OperatorBlocks::compileDivide)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_random", &OperatorBlocks::compilePickRandom)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_lt", &OperatorBlocks::compileLessThan)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_equals", &OperatorBlocks::compileEquals)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_gt", &OperatorBlocks::compileGreaterThan)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_and", &OperatorBlocks::compileAnd)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_or", &OperatorBlocks::compileOr)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_not", &OperatorBlocks::compileNot)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_join", &OperatorBlocks::compileJoin)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_letter_of", &OperatorBlocks::compileLetterOf)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_length", &OperatorBlocks::compileLength)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_contains", &OperatorBlocks::compileContains)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_mod", &OperatorBlocks::compileMod)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_round", &OperatorBlocks::compileRound)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "operator_mathop", &OperatorBlocks::compileMathOp)).Times(1);

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "NUM1", OperatorBlocks::NUM1)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "NUM2", OperatorBlocks::NUM2)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "FROM", OperatorBlocks::FROM)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "TO", OperatorBlocks::TO)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "OPERAND1", OperatorBlocks::OPERAND1)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "OPERAND2", OperatorBlocks::OPERAND2)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "OPERAND", OperatorBlocks::OPERAND)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "STRING1", OperatorBlocks::STRING1)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "STRING2", OperatorBlocks::STRING2)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "LETTER", OperatorBlocks::LETTER)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "STRING", OperatorBlocks::STRING)).Times(1);
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "NUM", OperatorBlocks::NUM)).Times(1);

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "OPERATOR", OperatorBlocks::OPERATOR));

    // Field values
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "abs", OperatorBlocks::Abs)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "floor", OperatorBlocks::Floor)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "ceiling", OperatorBlocks::Ceiling)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "sqrt", OperatorBlocks::Sqrt)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "sin", OperatorBlocks::Sin)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "cos", OperatorBlocks::Cos)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "tan", OperatorBlocks::Tan)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "asin", OperatorBlocks::Asin)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "acos", OperatorBlocks::Acos)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "atan", OperatorBlocks::Atan)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "ln", OperatorBlocks::Ln)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "log", OperatorBlocks::Log)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "e ^", OperatorBlocks::Eexp)).Times(1);
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "10 ^", OperatorBlocks::Op_10exp)).Times(1);

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(OperatorBlocksTest, Add)
{
    Compiler compiler(&m_engine);

    // 2 + 3
    auto block1 = createOperatorBlock("a", "operator_add");
    addInput(block1, "NUM1", OperatorBlocks::NUM1, 2);
    addInput(block1, "NUM2", OperatorBlocks::NUM2, 3);

    // 5.25 + 12.227
    auto block2 = createOperatorBlock("b", "operator_add");
    addInput(block2, "NUM1", OperatorBlocks::NUM1, 5.25);
    addInput(block2, "NUM2", OperatorBlocks::NUM2, 12.227);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileAdd(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileAdd(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_ADD, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_ADD, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 2, 3, 5.25, 12.227 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Subtract)
{
    Compiler compiler(&m_engine);

    // 5 - 2
    auto block1 = createOperatorBlock("a", "operator_subtract");
    addInput(block1, "NUM1", OperatorBlocks::NUM1, 5);
    addInput(block1, "NUM2", OperatorBlocks::NUM2, 2);

    // 50.3 - 0.25
    auto block2 = createOperatorBlock("b", "operator_subtract");
    addInput(block2, "NUM1", OperatorBlocks::NUM1, 50.3);
    addInput(block2, "NUM2", OperatorBlocks::NUM2, 0.25);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileSubtract(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileSubtract(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_SUBTRACT, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_SUBTRACT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 5, 2, 50.3, 0.25 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Multiply)
{
    Compiler compiler(&m_engine);

    // 2 * 5
    auto block1 = createOperatorBlock("a", "operator_multiply");
    addInput(block1, "NUM1", OperatorBlocks::NUM1, 2);
    addInput(block1, "NUM2", OperatorBlocks::NUM2, 5);

    // 1.2 * 0.12
    auto block2 = createOperatorBlock("b", "operator_multiply");
    addInput(block2, "NUM1", OperatorBlocks::NUM1, 1.2);
    addInput(block2, "NUM2", OperatorBlocks::NUM2, 0.12);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileMultiply(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileMultiply(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_MULTIPLY, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_MULTIPLY, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 2, 5, 1.2, 0.12 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Divide)
{
    Compiler compiler(&m_engine);

    // 3 / 2
    auto block1 = createOperatorBlock("a", "operator_divide");
    addInput(block1, "NUM1", OperatorBlocks::NUM1, 3);
    addInput(block1, "NUM2", OperatorBlocks::NUM2, 2);

    // -2 / 0.5
    auto block2 = createOperatorBlock("b", "operator_divide");
    addInput(block2, "NUM1", OperatorBlocks::NUM1, -2);
    addInput(block2, "NUM2", OperatorBlocks::NUM2, 0.5);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileDivide(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileDivide(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_DIVIDE, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_DIVIDE, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 3, 2, -2, 0.5 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Random)
{
    Compiler compiler(&m_engine);

    // pick random 1 to 10
    auto block1 = createOperatorBlock("a", "operator_random");
    addInput(block1, "FROM", OperatorBlocks::FROM, 1);
    addInput(block1, "TO", OperatorBlocks::TO, 10);

    // pick random -2.32 to 5.3
    auto block2 = createOperatorBlock("b", "operator_random");
    addInput(block2, "FROM", OperatorBlocks::FROM, -2.32);
    addInput(block2, "TO", OperatorBlocks::TO, 5.3);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compilePickRandom(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compilePickRandom(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_RANDOM, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_RANDOM, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 1, 10, -2.32, 5.3 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Lt)
{
    Compiler compiler(&m_engine);

    // 2 < 4
    auto block1 = createOperatorBlock("a", "operator_lt");
    addInput(block1, "OPERAND1", OperatorBlocks::OPERAND1, 2);
    addInput(block1, "OPERAND2", OperatorBlocks::OPERAND2, 4);

    // 81.5 < -35.221
    auto block2 = createOperatorBlock("b", "operator_lt");
    addInput(block2, "OPERAND1", OperatorBlocks::OPERAND1, 81.5);
    addInput(block2, "OPERAND2", OperatorBlocks::OPERAND2, -35.221);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileLessThan(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileLessThan(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_LESS_THAN, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_LESS_THAN, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 2, 4, 81.5, -35.221 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Equals)
{
    Compiler compiler(&m_engine);

    // 2 = 4
    auto block1 = createOperatorBlock("a", "operator_equals");
    addInput(block1, "OPERAND1", OperatorBlocks::OPERAND1, 2);
    addInput(block1, "OPERAND2", OperatorBlocks::OPERAND2, 4);

    // 81.5 = -35.221
    auto block2 = createOperatorBlock("b", "operator_equals");
    addInput(block2, "OPERAND1", OperatorBlocks::OPERAND1, 81.5);
    addInput(block2, "OPERAND2", OperatorBlocks::OPERAND2, -35.221);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileEquals(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileEquals(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_EQUALS, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_EQUALS, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 2, 4, 81.5, -35.221 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Gt)
{
    Compiler compiler(&m_engine);

    // 2 > 4
    auto block1 = createOperatorBlock("a", "operator_gt");
    addInput(block1, "OPERAND1", OperatorBlocks::OPERAND1, 2);
    addInput(block1, "OPERAND2", OperatorBlocks::OPERAND2, 4);

    // 81.5 > -35.221
    auto block2 = createOperatorBlock("b", "operator_gt");
    addInput(block2, "OPERAND1", OperatorBlocks::OPERAND1, 81.5);
    addInput(block2, "OPERAND2", OperatorBlocks::OPERAND2, -35.221);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileGreaterThan(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileGreaterThan(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_GREATER_THAN, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_GREATER_THAN, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 2, 4, 81.5, -35.221 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, And)
{
    Compiler compiler(&m_engine);

    // <> and <> (without inputs)
    auto block1 = createOperatorBlock("a", "operator_and");

    // <3 > 2> and <null> (second input is a null shadow)
    auto block2 = createOperatorBlock("b", "operator_and");
    auto gtBlock = createOperatorBlock("c", "operator_gt");
    gtBlock->setCompileFunction(&OperatorBlocks::compileGreaterThan);
    addInput(gtBlock, "OPERAND1", OperatorBlocks::OPERAND1, 3);
    addInput(gtBlock, "OPERAND2", OperatorBlocks::OPERAND2, 2);
    addObscuredInput(block2, "OPERAND1", OperatorBlocks::OPERAND1, gtBlock);
    addNullInput(block2, "OPERAND2", OperatorBlocks::OPERAND2);

    // <> and <1 < 10> (first input is missing)
    auto block3 = createOperatorBlock("d", "operator_and");
    auto ltBlock = createOperatorBlock("e", "operator_lt");
    ltBlock->setCompileFunction(&OperatorBlocks::compileLessThan);
    addInput(ltBlock, "OPERAND1", OperatorBlocks::OPERAND1, 1);
    addInput(ltBlock, "OPERAND2", OperatorBlocks::OPERAND2, 10);
    addObscuredInput(block3, "OPERAND2", OperatorBlocks::OPERAND2, ltBlock);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileAnd(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileAnd(&compiler);
    compiler.setBlock(block3);
    OperatorBlocks::compileAnd(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_NULL, vm::OP_NULL,  vm::OP_AND, vm::OP_CONST, 0, vm::OP_CONST,     1,          vm::OP_GREATER_THAN, vm::OP_CONST, 2,
              vm::OP_AND,   vm::OP_NULL, vm::OP_CONST, 3,          vm::OP_CONST, 4, vm::OP_LESS_THAN, vm::OP_AND, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 3, 2, Value(), 1, 10 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Or)
{
    Compiler compiler(&m_engine);

    // <> or <> (without inputs)
    auto block1 = createOperatorBlock("a", "operator_or");

    // <3 > 2> or <null> (second input is a null shadow)
    auto block2 = createOperatorBlock("b", "operator_or");
    auto gtBlock = createOperatorBlock("c", "operator_gt");
    gtBlock->setCompileFunction(&OperatorBlocks::compileGreaterThan);
    addInput(gtBlock, "OPERAND1", OperatorBlocks::OPERAND1, 3);
    addInput(gtBlock, "OPERAND2", OperatorBlocks::OPERAND2, 2);
    addObscuredInput(block2, "OPERAND1", OperatorBlocks::OPERAND1, gtBlock);
    addNullInput(block2, "OPERAND2", OperatorBlocks::OPERAND2);

    // <> or <1 < 10> (first input is missing)
    auto block3 = createOperatorBlock("d", "operator_or");
    auto ltBlock = createOperatorBlock("e", "operator_lt");
    ltBlock->setCompileFunction(&OperatorBlocks::compileLessThan);
    addInput(ltBlock, "OPERAND1", OperatorBlocks::OPERAND1, 1);
    addInput(ltBlock, "OPERAND2", OperatorBlocks::OPERAND2, 10);
    addObscuredInput(block3, "OPERAND2", OperatorBlocks::OPERAND2, ltBlock);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileOr(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileOr(&compiler);
    compiler.setBlock(block3);
    OperatorBlocks::compileOr(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_NULL, vm::OP_NULL,  vm::OP_OR, vm::OP_CONST, 0, vm::OP_CONST,     1,         vm::OP_GREATER_THAN, vm::OP_CONST, 2,
              vm::OP_OR,    vm::OP_NULL, vm::OP_CONST, 3,         vm::OP_CONST, 4, vm::OP_LESS_THAN, vm::OP_OR, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 3, 2, Value(), 1, 10 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Not)
{
    Compiler compiler(&m_engine);

    // <> (without any input)
    auto block1 = createOperatorBlock("a", "operator_not");

    // <3 > 2>
    auto block2 = createOperatorBlock("b", "operator_not");
    auto gtBlock = createOperatorBlock("c", "operator_gt");
    gtBlock->setCompileFunction(&OperatorBlocks::compileGreaterThan);
    addInput(gtBlock, "OPERAND1", OperatorBlocks::OPERAND1, 3);
    addInput(gtBlock, "OPERAND2", OperatorBlocks::OPERAND2, 2);
    addObscuredInput(block2, "OPERAND", OperatorBlocks::OPERAND, gtBlock);

    // <null> (the input is a null shadow)
    auto block3 = createOperatorBlock("d", "operator_not");
    addNullInput(block3, "OPERAND", OperatorBlocks::OPERAND);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileNot(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileNot(&compiler);
    compiler.setBlock(block3);
    OperatorBlocks::compileNot(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_NULL, vm::OP_NOT, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_GREATER_THAN, vm::OP_NOT, vm::OP_CONST, 2, vm::OP_NOT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 3, 2, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Join)
{
    Compiler compiler(&m_engine);

    // join "hello " "world"
    auto block1 = createOperatorBlock("a", "operator_join");
    addInput(block1, "STRING1", OperatorBlocks::STRING1, "hello ");
    addInput(block1, "STRING2", OperatorBlocks::STRING2, "world");

    // join "test_" 50.01
    auto block2 = createOperatorBlock("b", "operator_join");
    addInput(block2, "STRING1", OperatorBlocks::STRING1, "test_");
    addInput(block2, "STRING2", OperatorBlocks::STRING2, 50.01);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileJoin(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileJoin(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_STR_CONCAT, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_STR_CONCAT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "hello ", "world", "test_", 50.01 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, LetterOf)
{
    Compiler compiler(&m_engine);

    // letter 5 of "test"
    auto block1 = createOperatorBlock("a", "operator_letter_of");
    addInput(block1, "LETTER", OperatorBlocks::LETTER, 5);
    addInput(block1, "STRING", OperatorBlocks::STRING, "test");

    // join 7 of -85.108
    auto block2 = createOperatorBlock("b", "operator_letter_of");
    addInput(block2, "LETTER", OperatorBlocks::LETTER, 7);
    addInput(block2, "STRING", OperatorBlocks::STRING, -85.108);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileLetterOf(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileLetterOf(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_STR_AT, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_STR_AT, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "test", 5, -85.108, 7 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Length)
{
    Compiler compiler(&m_engine);

    // length of "test"
    auto block1 = createOperatorBlock("a", "operator_length");
    addInput(block1, "STRING", OperatorBlocks::STRING, "test");

    // length of -85.108
    auto block2 = createOperatorBlock("b", "operator_length");
    addInput(block2, "STRING", OperatorBlocks::STRING, -85.108);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileLength(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileLength(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_STR_LENGTH, vm::OP_CONST, 1, vm::OP_STR_LENGTH, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "test", -85.108 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Contains)
{
    Compiler compiler(&m_engine);

    // "hello" contains "e"
    auto block1 = createOperatorBlock("a", "operator_contains");
    addInput(block1, "STRING1", OperatorBlocks::STRING1, "hello");
    addInput(block1, "STRING2", OperatorBlocks::STRING2, "e");

    // 50.01 contains "1"
    auto block2 = createOperatorBlock("b", "operator_contains");
    addInput(block2, "STRING1", OperatorBlocks::STRING1, 50.01);
    addInput(block2, "STRING2", OperatorBlocks::STRING2, "1");

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileContains(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileContains(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_STR_CONTAINS, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_STR_CONTAINS, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "hello", "e", 50.01, "1" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Mod)
{
    Compiler compiler(&m_engine);

    // 7 mod 5
    auto block1 = createOperatorBlock("a", "operator_mod");
    addInput(block1, "NUM1", OperatorBlocks::NUM1, 7);
    addInput(block1, "NUM2", OperatorBlocks::NUM2, 5);

    // -5.25 mod 3.5
    auto block2 = createOperatorBlock("b", "operator_mod");
    addInput(block2, "NUM1", OperatorBlocks::NUM1, -5.25);
    addInput(block2, "NUM2", OperatorBlocks::NUM2, 3.5);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileMod(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileMod(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 1, vm::OP_MOD, vm::OP_CONST, 2, vm::OP_CONST, 3, vm::OP_MOD, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 7, 5, -5.25, 3.5 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, Round)
{
    Compiler compiler(&m_engine);

    // round 6
    auto block1 = createOperatorBlock("a", "operator_round");
    addInput(block1, "NUM", OperatorBlocks::NUM, 6);

    // round -2.054
    auto block2 = createOperatorBlock("b", "operator_round");
    addInput(block2, "NUM", OperatorBlocks::NUM, -2.054);

    compiler.init();
    compiler.setBlock(block1);
    OperatorBlocks::compileRound(&compiler);
    compiler.setBlock(block2);
    OperatorBlocks::compileRound(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_ROUND, vm::OP_CONST, 1, vm::OP_ROUND, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 6, -2.054 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, MathOp)
{
    Compiler compiler(&m_engineMock);
    std::vector<std::shared_ptr<Block>> blocks;
    std::shared_ptr<Block> block;

    // abs of -5
    block = createOperatorBlock("a", "operator_mathop");
    addMathOpField(block, "abs", OperatorBlocks::Abs);
    addInput(block, "NUM", OperatorBlocks::NUM, -5);
    blocks.push_back(block);

    // floor of 2.054
    block = createOperatorBlock("b", "operator_mathop");
    addMathOpField(block, "floor", OperatorBlocks::Floor);
    addInput(block, "NUM", OperatorBlocks::NUM, 2.054);
    blocks.push_back(block);

    // ceiling of 8.2
    block = createOperatorBlock("c", "operator_mathop");
    addMathOpField(block, "ceiling", OperatorBlocks::Ceiling);
    addInput(block, "NUM", OperatorBlocks::NUM, 8.2);
    blocks.push_back(block);

    // sqrt of 25
    block = createOperatorBlock("d", "operator_mathop");
    addMathOpField(block, "sqrt", OperatorBlocks::Sqrt);
    addInput(block, "NUM", OperatorBlocks::NUM, 25);
    blocks.push_back(block);

    // sin of 45
    block = createOperatorBlock("e", "operator_mathop");
    addMathOpField(block, "sin", OperatorBlocks::Sin);
    addInput(block, "NUM", OperatorBlocks::NUM, 45);
    blocks.push_back(block);

    // cos of 90
    block = createOperatorBlock("f", "operator_mathop");
    addMathOpField(block, "cos", OperatorBlocks::Cos);
    addInput(block, "NUM", OperatorBlocks::NUM, 90);
    blocks.push_back(block);

    // tan of 10.5
    block = createOperatorBlock("g", "operator_mathop");
    addMathOpField(block, "tan", OperatorBlocks::Tan);
    addInput(block, "NUM", OperatorBlocks::NUM, 10.5);
    blocks.push_back(block);

    // asin of 0.5
    block = createOperatorBlock("h", "operator_mathop");
    addMathOpField(block, "asin", OperatorBlocks::Asin);
    addInput(block, "NUM", OperatorBlocks::NUM, 0.5);
    blocks.push_back(block);

    // acos of 0
    block = createOperatorBlock("i", "operator_mathop");
    addMathOpField(block, "acos", OperatorBlocks::Acos);
    addInput(block, "NUM", OperatorBlocks::NUM, 0);
    blocks.push_back(block);

    // atan of 20
    block = createOperatorBlock("j", "operator_mathop");
    addMathOpField(block, "atan", OperatorBlocks::Atan);
    addInput(block, "NUM", OperatorBlocks::NUM, 20);
    blocks.push_back(block);

    // ln of 3
    block = createOperatorBlock("k", "operator_mathop");
    addMathOpField(block, "ln", OperatorBlocks::Ln);
    addInput(block, "NUM", OperatorBlocks::NUM, 3);
    blocks.push_back(block);

    // log of 100
    block = createOperatorBlock("l", "operator_mathop");
    addMathOpField(block, "log", OperatorBlocks::Log);
    addInput(block, "NUM", OperatorBlocks::NUM, 100);
    blocks.push_back(block);

    // e ^ of 100
    block = createOperatorBlock("m", "operator_mathop");
    addMathOpField(block, "e ^", OperatorBlocks::Eexp);
    addInput(block, "NUM", OperatorBlocks::NUM, 100);
    blocks.push_back(block);

    // 10 ^ of 4
    block = createOperatorBlock("n", "operator_mathop");
    addMathOpField(block, "10 ^", OperatorBlocks::Op_10exp);
    addInput(block, "NUM", OperatorBlocks::NUM, 4);
    blocks.push_back(block);

    compiler.init();

    for (auto block : blocks) {
        compiler.setBlock(block);

        switch (block->fieldAt(0)->specialValueId()) {
            case OperatorBlocks::Ln:
                EXPECT_CALL(m_engineMock, functionIndex(&OperatorBlocks::op_ln)).WillOnce(Return(0));
                break;
            case OperatorBlocks::Log:
                EXPECT_CALL(m_engineMock, functionIndex(&OperatorBlocks::op_log)).WillOnce(Return(1));
                break;
            case OperatorBlocks::Eexp:
                EXPECT_CALL(m_engineMock, functionIndex(&OperatorBlocks::op_eexp)).WillOnce(Return(2));
                break;
            case OperatorBlocks::Op_10exp:
                EXPECT_CALL(m_engineMock, functionIndex(&OperatorBlocks::op_10exp)).WillOnce(Return(3));
                break;
            default:
                break;
        }

        OperatorBlocks::compileMathOp(&compiler);
    }

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_ABS,
              vm::OP_CONST,
              1,
              vm::OP_FLOOR,
              vm::OP_CONST,
              2,
              vm::OP_CEIL,
              vm::OP_CONST,
              3,
              vm::OP_SQRT,
              vm::OP_CONST,
              4,
              vm::OP_SIN,
              vm::OP_CONST,
              5,
              vm::OP_COS,
              vm::OP_CONST,
              6,
              vm::OP_TAN,
              vm::OP_CONST,
              7,
              vm::OP_ASIN,
              vm::OP_CONST,
              8,
              vm::OP_ACOS,
              vm::OP_CONST,
              9,
              vm::OP_ATAN,
              vm::OP_CONST,
              10,
              vm::OP_EXEC,
              0,
              vm::OP_CONST,
              11,
              vm::OP_EXEC,
              1,
              vm::OP_CONST,
              12,
              vm::OP_EXEC,
              2,
              vm::OP_CONST,
              13,
              vm::OP_EXEC,
              3,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ -5, 2.054, 8.2, 25, 45, 90, 10.5, 0.5, 0, 20, 3, 100, 100, 4 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(OperatorBlocksTest, MathOpLn)
{
    static unsigned int bytecode[] = {
        vm::OP_START,
        vm::OP_CONST,
        0,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        1,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        2,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        3,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        4,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        5,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        6,
        vm::OP_EXEC,
        0,
        vm::OP_HALT
    };
    static BlockFunc functions[] = { &OperatorBlocks::op_ln };
    static Value constValues[] = { SpecialValue::NegativeInfinity, -2.5, 0, SpecialValue::NaN, std::exp(1), 50, SpecialValue::Infinity };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 7);
    ASSERT_TRUE(vm.getInput(0, 7)->isNaN());
    ASSERT_TRUE(vm.getInput(1, 7)->isNaN());
    ASSERT_TRUE(vm.getInput(2, 7)->isNegativeInfinity());
    ASSERT_TRUE(vm.getInput(3, 7)->isNegativeInfinity());
    ASSERT_EQ(vm.getInput(4, 7)->toDouble(), 1);
    ASSERT_EQ(std::round(vm.getInput(5, 7)->toDouble() * 1000) / 1000, 3.912);
    ASSERT_TRUE(vm.getInput(6, 7)->isInfinity());
}

TEST_F(OperatorBlocksTest, MathOpLog)
{
    static unsigned int bytecode[] = {
        vm::OP_START,
        vm::OP_CONST,
        0,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        1,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        2,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        3,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        4,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        5,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        6,
        vm::OP_EXEC,
        0,
        vm::OP_HALT
    };
    static BlockFunc functions[] = { &OperatorBlocks::op_log };
    static Value constValues[] = { SpecialValue::NegativeInfinity, -2.5, 0, SpecialValue::NaN, 100, 1500, SpecialValue::Infinity };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 7);
    ASSERT_TRUE(vm.getInput(0, 7)->isNaN());
    ASSERT_TRUE(vm.getInput(1, 7)->isNaN());
    ASSERT_TRUE(vm.getInput(2, 7)->isNegativeInfinity());
    ASSERT_TRUE(vm.getInput(3, 7)->isNegativeInfinity());
    ASSERT_EQ(vm.getInput(4, 7)->toDouble(), 2);
    ASSERT_EQ(std::round(vm.getInput(5, 7)->toDouble() * 1000) / 1000, 3.176);
    ASSERT_TRUE(vm.getInput(6, 7)->isInfinity());
}

TEST_F(OperatorBlocksTest, MathOpExp)
{
    static unsigned int bytecode[] = {
        vm::OP_START,
        vm::OP_CONST,
        0,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        1,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        2,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        3,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        4,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        5,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        6,
        vm::OP_EXEC,
        0,
        vm::OP_HALT
    };
    static BlockFunc functions[] = { &OperatorBlocks::op_eexp };
    static Value constValues[] = { SpecialValue::NegativeInfinity, -3.25, 0, SpecialValue::NaN, 1, 5, SpecialValue::Infinity };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 7);
    ASSERT_EQ(vm.getInput(0, 7)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(1, 7)->toDouble(), std::exp(-3.25));
    ASSERT_EQ(vm.getInput(2, 7)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(3, 7)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(4, 7)->toDouble(), std::exp(1));
    ASSERT_EQ(vm.getInput(5, 7)->toDouble(), std::exp(5));
    ASSERT_TRUE(vm.getInput(6, 7)->isInfinity());
}

TEST_F(OperatorBlocksTest, MathOp10Exp)
{
    static unsigned int bytecode[] = {
        vm::OP_START,
        vm::OP_CONST,
        0,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        1,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        2,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        3,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        4,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        5,
        vm::OP_EXEC,
        0,
        vm::OP_CONST,
        6,
        vm::OP_EXEC,
        0,
        vm::OP_HALT
    };
    static BlockFunc functions[] = { &OperatorBlocks::op_10exp };
    static Value constValues[] = { SpecialValue::NegativeInfinity, -2, 0, SpecialValue::NaN, 1, 5.5, SpecialValue::Infinity };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 7);
    ASSERT_EQ(vm.getInput(0, 7)->toDouble(), 0);
    ASSERT_EQ(vm.getInput(1, 7)->toDouble(), 0.01);
    ASSERT_EQ(vm.getInput(2, 7)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(3, 7)->toDouble(), 1);
    ASSERT_EQ(vm.getInput(4, 7)->toDouble(), 10);
    ASSERT_EQ(vm.getInput(5, 7)->toDouble(), std::pow(10, 5.5));
    ASSERT_TRUE(vm.getInput(6, 7)->isInfinity());
}
