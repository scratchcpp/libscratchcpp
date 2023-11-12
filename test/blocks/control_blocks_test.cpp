#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/sprite.h>
#include <enginemock.h>
#include <clockmock.h>

#include "../common.h"
#include "blocks/controlblocks.h"
#include "blocks/operatorblocks.h"
#include "engine/internal/engine.h"
#include "engine/internal/clock.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;

class ControlBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<ControlBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For any control block
        std::shared_ptr<Block> createControlBlock(const std::string &id, const std::string &opcode) const { return std::make_shared<Block>(id, opcode); }

        // For control_create_clone_of
        std::shared_ptr<Block> createCloneBlock(const std::string &id, const std::string &spriteName, std::shared_ptr<Block> valueBlock = nullptr)
        {
            auto block = createControlBlock(id, "control_create_clone_of");

            if (valueBlock)
                addObscuredInput(block, "CLONE_OPTION", ControlBlocks::CLONE_OPTION, valueBlock);
            else {
                auto input = addNullInput(block, "CLONE_OPTION", ControlBlocks::CLONE_OPTION);
                auto menu = createControlBlock(id + "_menu", "control_create_clone_of_menu");
                input->setValueBlock(menu);
                addDropdownField(menu, "CLONE_OPTION", static_cast<ControlBlocks::Fields>(-1), spriteName, static_cast<ControlBlocks::FieldValues>(-1));
            }

            return block;
        }

        std::shared_ptr<Block> createNullBlock(const std::string &id)
        {
            std::shared_ptr<Block> block = std::make_shared<Block>(id, "");
            BlockComp func = [](Compiler *compiler) { compiler->addInstruction(vm::OP_NULL); };
            block->setCompileFunction(func);

            return block;
        }

        void addSubstackInput(std::shared_ptr<Block> block, const std::string &name, ControlBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::NoShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, ControlBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, ControlBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, ControlBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);

            return input;
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, ControlBlocks::Fields id, const std::string &value, ControlBlocks::FieldValues valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
        }

        void addVariableField(std::shared_ptr<Block> block, std::shared_ptr<Variable> variable)
        {
            auto variableField = std::make_shared<Field>("VARIABLE", Value(), variable);
            variableField->setFieldId(ControlBlocks::VARIABLE);
            block->addField(variableField);
        }

        std::shared_ptr<Block> createSubstack(const std::string &id)
        {
            auto block = std::make_shared<Block>("b", "some_block");

            block->setCompileFunction([](Compiler *compiler) {
                compiler->addInstruction(vm::OP_NULL);
                compiler->addInstruction(vm::OP_PRINT);
            });

            return block;
        }

        std::shared_ptr<Block> createSubstack2(const std::string &id)
        {
            auto block = std::make_shared<Block>("b", "some_block");

            block->setCompileFunction([](Compiler *compiler) {
                compiler->addInstruction(vm::OP_NULL);
                compiler->addInstruction(vm::OP_NOT); // to distinguish from substack1
                compiler->addInstruction(vm::OP_PRINT);
            });

            return block;
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(ControlBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Control");
}

TEST_F(ControlBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(ControlBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_forever", &ControlBlocks::compileRepeatForever)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_repeat", &ControlBlocks::compileRepeat)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_repeat_until", &ControlBlocks::compileRepeatUntil)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_while", &ControlBlocks::compileRepeatWhile)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_for_each", &ControlBlocks::compileRepeatForEach)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_if", &ControlBlocks::compileIfStatement)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_if_else", &ControlBlocks::compileIfElseStatement)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_stop", &ControlBlocks::compileStop)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_wait", &ControlBlocks::compileWait)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_wait_until", &ControlBlocks::compileWaitUntil)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_start_as_clone", &ControlBlocks::compileStartAsClone)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_create_clone_of", &ControlBlocks::compileCreateClone)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "control_delete_this_clone", &ControlBlocks::compileDeleteThisClone)).Times(1);

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SUBSTACK", ControlBlocks::SUBSTACK));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SUBSTACK2", ControlBlocks::SUBSTACK2));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "TIMES", ControlBlocks::TIMES));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "CONDITION", ControlBlocks::CONDITION));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DURATION", ControlBlocks::DURATION));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "VALUE", ControlBlocks::VALUE));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "CLONE_OPTION", ControlBlocks::CLONE_OPTION));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "STOP_OPTION", ControlBlocks::STOP_OPTION));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "VARIABLE", ControlBlocks::VARIABLE));

    // Field values
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "all", ControlBlocks::StopAll));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "this script", ControlBlocks::StopThisScript));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "other scripts in sprite", ControlBlocks::StopOtherScriptsInSprite));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(ControlBlocksTest, Forever)
{
    Compiler compiler(&m_engine);

    // with substack
    auto block1 = createControlBlock("a", "control_forever");
    auto substack = createSubstack("b");
    addSubstackInput(block1, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    block1->setCompileFunction(&ControlBlocks::compileRepeatForever);

    // with null substack
    auto block2 = createControlBlock("c", "control_forever");
    addNullInput(block2, "SUBSTACK", ControlBlocks::SUBSTACK);
    block2->setCompileFunction(&ControlBlocks::compileRepeatForever);

    // without substack
    auto block3 = createControlBlock("d", "control_forever");
    block3->setCompileFunction(&ControlBlocks::compileRepeatForever);

    compiler.compile(block1);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_FOREVER_LOOP, vm::OP_NULL, vm::OP_PRINT, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_FOREVER_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_FOREVER_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, Repeat)
{
    Compiler compiler(&m_engine);

    // with substack
    auto block1 = createControlBlock("a", "control_repeat");
    auto substack = createSubstack("b");
    addSubstackInput(block1, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    addValueInput(block1, "TIMES", ControlBlocks::TIMES, 5);
    block1->setCompileFunction(&ControlBlocks::compileRepeat);

    // with null substack
    auto block2 = createControlBlock("c", "control_repeat");
    addNullInput(block2, "SUBSTACK", ControlBlocks::SUBSTACK);
    addValueInput(block2, "TIMES", ControlBlocks::TIMES, 10);
    block2->setCompileFunction(&ControlBlocks::compileRepeat);

    // without substack
    auto block3 = createControlBlock("d", "control_repeat");
    addValueInput(block3, "TIMES", ControlBlocks::TIMES, 8);
    block3->setCompileFunction(&ControlBlocks::compileRepeat);

    compiler.compile(block1);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_REPEAT_LOOP, vm::OP_NULL, vm::OP_PRINT, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, RepeatUntil)
{
    Compiler compiler(&m_engine);

    // with substack
    auto block1 = createControlBlock("a", "control_repeat_until");
    auto substack = createSubstack("b");
    addSubstackInput(block1, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    addValueInput(block1, "CONDITION", ControlBlocks::CONDITION, false);
    block1->setCompileFunction(&ControlBlocks::compileRepeatUntil);

    // with null substack
    auto block2 = createControlBlock("c", "control_repeat_until");
    addNullInput(block2, "SUBSTACK", ControlBlocks::SUBSTACK);
    addValueInput(block2, "CONDITION", ControlBlocks::CONDITION, false);
    block2->setCompileFunction(&ControlBlocks::compileRepeatUntil);

    // without substack
    auto block3 = createControlBlock("d", "control_repeat_until");
    addValueInput(block3, "CONDITION", ControlBlocks::CONDITION, false);
    block3->setCompileFunction(&ControlBlocks::compileRepeatUntil);

    // without substack and with null condition
    auto block4 = createControlBlock("e", "control_repeat_until");
    addNullInput(block4, "CONDITION", ControlBlocks::CONDITION);
    block4->setCompileFunction(&ControlBlocks::compileRepeatUntil);

    // without substack and without condition
    auto block5 = createControlBlock("f", "control_repeat_until");
    block5->setCompileFunction(&ControlBlocks::compileRepeatUntil);

    compiler.compile(block1);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 0, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_NULL, vm::OP_PRINT, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toBool(), false);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 1, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 2, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block4);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 3, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block5);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_NULL, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, While)
{
    Compiler compiler(&m_engine);

    // with substack
    auto block1 = createControlBlock("a", "control_while");
    auto substack = createSubstack("b");
    addSubstackInput(block1, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    addValueInput(block1, "CONDITION", ControlBlocks::CONDITION, false);
    block1->setCompileFunction(&ControlBlocks::compileRepeatWhile);

    // with null substack
    auto block2 = createControlBlock("c", "control_while");
    addNullInput(block2, "SUBSTACK", ControlBlocks::SUBSTACK);
    addValueInput(block2, "CONDITION", ControlBlocks::CONDITION, false);
    block2->setCompileFunction(&ControlBlocks::compileRepeatWhile);

    // without substack
    auto block3 = createControlBlock("d", "control_while");
    addValueInput(block3, "CONDITION", ControlBlocks::CONDITION, false);
    block3->setCompileFunction(&ControlBlocks::compileRepeatWhile);

    // without substack and with null condition
    auto block4 = createControlBlock("e", "control_while");
    addNullInput(block4, "CONDITION", ControlBlocks::CONDITION);
    block4->setCompileFunction(&ControlBlocks::compileRepeatWhile);

    // without substack and without condition
    auto block5 = createControlBlock("f", "control_while");
    block5->setCompileFunction(&ControlBlocks::compileRepeatWhile);

    compiler.compile(block1);

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 0, vm::OP_NOT, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_NULL, vm::OP_PRINT, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toBool(), false);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 1, vm::OP_NOT, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 2, vm::OP_NOT, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block4);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_CONST, 3, vm::OP_NOT, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block5);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_UNTIL_LOOP, vm::OP_NULL, vm::OP_NOT, vm::OP_BEGIN_UNTIL_LOOP, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, ForEach)
{
    Compiler compiler(&m_engine);
    auto var = std::make_shared<Variable>("v", "var");

    // with substack
    auto block1 = createControlBlock("a", "control_for_each");
    auto substack = createSubstack("b");
    addSubstackInput(block1, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    addVariableField(block1, var);
    addValueInput(block1, "VALUE", ControlBlocks::VALUE, 5);
    block1->setCompileFunction(&ControlBlocks::compileRepeatForEach);

    // with null substack
    auto block2 = createControlBlock("c", "control_for_each");
    addNullInput(block2, "SUBSTACK", ControlBlocks::SUBSTACK);
    addVariableField(block2, var);
    addValueInput(block2, "VALUE", ControlBlocks::VALUE, 10);
    block2->setCompileFunction(&ControlBlocks::compileRepeatForEach);

    // without substack
    auto block3 = createControlBlock("d", "control_for_each");
    addVariableField(block3, var);
    addValueInput(block3, "VALUE", ControlBlocks::VALUE, 8);
    block3->setCompileFunction(&ControlBlocks::compileRepeatForEach);

    compiler.compile(block1);

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_REPEAT_LOOP, vm::OP_REPEAT_LOOP_INDEX1, vm::OP_SET_VAR, 0, vm::OP_NULL, vm::OP_PRINT, vm::OP_LOOP_END, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);
    ASSERT_EQ(compiler.variables().size(), 1);
    ASSERT_EQ(compiler.variables()[0], var.get());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 1, vm::OP_SET_VAR, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 5, 10 }));
    ASSERT_EQ(compiler.variables().size(), 1);
    ASSERT_EQ(compiler.variables()[0], var.get());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 2, vm::OP_SET_VAR, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 5, 10, 8 }));
    ASSERT_EQ(compiler.variables().size(), 1);
    ASSERT_EQ(compiler.variables()[0], var.get());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, If)
{
    Compiler compiler(&m_engine);

    // with substack
    auto block1 = createControlBlock("a", "control_if");
    auto substack = createSubstack("b");
    addSubstackInput(block1, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    addValueInput(block1, "CONDITION", ControlBlocks::CONDITION, false);
    block1->setCompileFunction(&ControlBlocks::compileIfStatement);

    // with null substack
    auto block2 = createControlBlock("c", "control_if");
    addNullInput(block2, "SUBSTACK", ControlBlocks::SUBSTACK);
    addValueInput(block2, "CONDITION", ControlBlocks::CONDITION, false);
    block2->setCompileFunction(&ControlBlocks::compileIfStatement);

    // without substack
    auto block3 = createControlBlock("d", "control_if");
    addValueInput(block3, "CONDITION", ControlBlocks::CONDITION, false);
    block3->setCompileFunction(&ControlBlocks::compileIfStatement);

    // with substack and with null condition
    auto block4 = createControlBlock("e", "control_if");
    substack = createSubstack("f");
    addSubstackInput(block4, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    addNullInput(block4, "CONDITION", ControlBlocks::CONDITION);
    block4->setCompileFunction(&ControlBlocks::compileIfStatement);

    // with substack and without condition
    auto block5 = createControlBlock("g", "control_if");
    substack = createSubstack("h");
    addSubstackInput(block5, "SUBSTACK", ControlBlocks::SUBSTACK, substack);
    block5->setCompileFunction(&ControlBlocks::compileIfStatement);

    compiler.compile(block1);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_IF, vm::OP_NULL, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toBool(), false);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toBool(), false);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toBool(), false);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block4);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 1, vm::OP_IF, vm::OP_NULL, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block5);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_NULL, vm::OP_IF, vm::OP_NULL, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, IfElse)
{
    Compiler compiler(&m_engine);

    // with both substacks
    auto block1 = createControlBlock("a", "control_if_else");
    auto substack1 = createSubstack("b");
    addSubstackInput(block1, "SUBSTACK", ControlBlocks::SUBSTACK, substack1);
    auto substack2 = createSubstack2("c");
    addSubstackInput(block1, "SUBSTACK2", ControlBlocks::SUBSTACK2, substack2);
    addValueInput(block1, "CONDITION", ControlBlocks::CONDITION, false);
    block1->setCompileFunction(&ControlBlocks::compileIfElseStatement);

    // without first substack
    auto block2 = createControlBlock("d", "control_if_else");
    substack2 = createSubstack2("e");
    addSubstackInput(block2, "SUBSTACK2", ControlBlocks::SUBSTACK2, substack2);
    addValueInput(block2, "CONDITION", ControlBlocks::CONDITION, false);
    block2->setCompileFunction(&ControlBlocks::compileIfElseStatement);

    // without second substack
    auto block3 = createControlBlock("f", "control_if_else");
    substack1 = createSubstack("g");
    addSubstackInput(block3, "SUBSTACK", ControlBlocks::SUBSTACK, substack1);
    addValueInput(block3, "CONDITION", ControlBlocks::CONDITION, false);
    block3->setCompileFunction(&ControlBlocks::compileIfElseStatement);

    // without any substack
    auto block4 = createControlBlock("h", "control_if_else");
    addValueInput(block4, "CONDITION", ControlBlocks::CONDITION, false);
    block4->setCompileFunction(&ControlBlocks::compileIfElseStatement);

    // with both substacks and with null condition
    auto block5 = createControlBlock("i", "control_if_else");
    substack1 = createSubstack("j");
    addSubstackInput(block5, "SUBSTACK", ControlBlocks::SUBSTACK, substack1);
    substack2 = createSubstack2("k");
    addSubstackInput(block5, "SUBSTACK2", ControlBlocks::SUBSTACK2, substack2);
    addNullInput(block5, "CONDITION", ControlBlocks::CONDITION);
    block5->setCompileFunction(&ControlBlocks::compileIfElseStatement);

    // with both substacks and without condition
    auto block6 = createControlBlock("i", "control_if_else");
    substack1 = createSubstack("j");
    addSubstackInput(block6, "SUBSTACK", ControlBlocks::SUBSTACK, substack1);
    substack2 = createSubstack2("k");
    addSubstackInput(block6, "SUBSTACK2", ControlBlocks::SUBSTACK2, substack2);
    block6->setCompileFunction(&ControlBlocks::compileIfElseStatement);

    compiler.compile(block1);

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_IF, vm::OP_NULL, vm::OP_PRINT, vm::OP_ELSE, vm::OP_NULL, vm::OP_NOT, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toBool(), false);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block2);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 1, vm::OP_NOT, vm::OP_IF, vm::OP_NULL, vm::OP_NOT, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block3);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 2, vm::OP_IF, vm::OP_NULL, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block4);

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block5);

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 3, vm::OP_IF, vm::OP_NULL, vm::OP_PRINT, vm::OP_ELSE, vm::OP_NULL, vm::OP_NOT, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());

    compiler.compile(block6);

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_NULL, vm::OP_IF, vm::OP_NULL, vm::OP_PRINT, vm::OP_ELSE, vm::OP_NULL, vm::OP_NOT, vm::OP_PRINT, vm::OP_ENDIF, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ false, false, false, Value() }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, Stop)
{
    Compiler compiler(&m_engineMock);

    // stop [all]
    auto block1 = createControlBlock("a", "control_stop");
    addDropdownField(block1, "STOP_OPTION", ControlBlocks::STOP_OPTION, "all", ControlBlocks::StopAll);

    // stop [this script]
    auto block2 = createControlBlock("b", "control_stop");
    addDropdownField(block2, "STOP_OPTION", ControlBlocks::STOP_OPTION, "this script", ControlBlocks::StopThisScript);

    // stop [other scripts in sprite]
    auto block3 = createControlBlock("c", "control_stop");
    addDropdownField(block3, "STOP_OPTION", ControlBlocks::STOP_OPTION, "other scripts in sprite", ControlBlocks::StopOtherScriptsInSprite);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::stopAll)).WillOnce(Return(0));
    compiler.setBlock(block1);
    ControlBlocks::compileStop(&compiler);

    compiler.setBlock(block2);
    ControlBlocks::compileStop(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::stopOtherScriptsInSprite)).WillOnce(Return(1));
    compiler.setBlock(block3);
    ControlBlocks::compileStop(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT, vm::OP_HALT, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, StopAll)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &ControlBlocks::stopAll };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setBytecode(bytecode);

    EXPECT_CALL(m_engineMock, stop()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST_F(ControlBlocksTest, StopOtherScriptsInSprite)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &ControlBlocks::stopOtherScriptsInSprite };

    Target target;
    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setBytecode(bytecode);

    EXPECT_CALL(m_engineMock, stopTarget(&target, &vm)).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST_F(ControlBlocksTest, Wait)
{
    Compiler compiler(&m_engineMock);

    // wait 5 seconds
    auto block = createControlBlock("a", "control_wait");
    addValueInput(block, "DURATION", ControlBlocks::DURATION, 5);

    testing::Expectation startWait = EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::startWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::wait)).After(startWait).WillOnce(Return(1));

    compiler.init();
    compiler.setBlock(block);
    ControlBlocks::compileWait(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0], 5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, WaitImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &ControlBlocks::startWait, &ControlBlocks::wait };
    static Value constValues[] = { 5.5 };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.setBytecode(bytecode);

    ClockMock clock;
    ControlBlocks::clock = &clock;

    std::chrono::steady_clock::time_point startTime(std::chrono::milliseconds(1000));
    EXPECT_CALL(clock, currentSteadyTime()).Times(2).WillRepeatedly(Return(startTime));
    EXPECT_CALL(m_engineMock, lockFrame()).Times(1);
    EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(ControlBlocks::m_timeMap.find(&vm) != ControlBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());

    std::chrono::steady_clock::time_point time1(std::chrono::milliseconds(6450));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time1));
    EXPECT_CALL(m_engineMock, lockFrame()).Times(1);
    EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(ControlBlocks::m_timeMap.find(&vm) != ControlBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());

    std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(6500));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    EXPECT_CALL(m_engineMock, lockFrame()).Times(1);
    EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(ControlBlocks::m_timeMap.find(&vm) == ControlBlocks::m_timeMap.cend());
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(m_engineMock, lockFrame()).Times(0);
    EXPECT_CALL(m_engineMock, breakFrame()).Times(0);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(ControlBlocks::m_timeMap.find(&vm) == ControlBlocks::m_timeMap.cend());
    ASSERT_TRUE(vm.atEnd());

    ControlBlocks::clock = Clock::instance().get();
}

TEST_F(ControlBlocksTest, WaitUntil)
{
    Compiler compiler(&m_engineMock);

    // wait until <>
    auto block1 = createControlBlock("a", "control_wait_until");

    // wait until <null>
    auto block2 = createControlBlock("b", "control_wait_until");
    addNullInput(block2, "CONDITION", ControlBlocks::CONDITION);

    // wait until <true>
    auto block3 = createControlBlock("c", "control_wait_until");
    addValueInput(block3, "CONDITION", ControlBlocks::CONDITION, true);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::waitUntil)).WillOnce(Return(0));
    compiler.setBlock(block1);
    ControlBlocks::compileWaitUntil(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::waitUntil)).WillOnce(Return(0));
    compiler.setBlock(block2);
    ControlBlocks::compileWaitUntil(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::waitUntil)).WillOnce(Return(0));
    compiler.setBlock(block3);
    ControlBlocks::compileWaitUntil(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_CHECKPOINT, vm::OP_NULL, vm::OP_EXEC, 0, vm::OP_CHECKPOINT, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CHECKPOINT, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ Value(), true }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, WaitUntilImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CHECKPOINT, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &ControlBlocks::waitUntil };
    static Value constValuesBefore[] = { false };
    static Value constValuesAfter[] = { true };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValuesBefore);
    vm.setBytecode(bytecode);

    EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(m_engineMock, breakFrame()).Times(1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(vm.atEnd());

    EXPECT_CALL(m_engineMock, breakFrame()).Times(0);
    vm.setConstValues(constValuesAfter);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(vm.atEnd());
}

TEST_F(ControlBlocksTest, CreateCloneOf)
{
    Compiler compiler(&m_engineMock);

    // create clone of [Sprite1]
    auto block1 = createCloneBlock("a", "Sprite1");

    // create clone of [myself]
    auto block2 = createCloneBlock("b", "_myself_");

    // create clone of (null block)
    auto block3 = createCloneBlock("c", "", createNullBlock("d"));

    EXPECT_CALL(m_engineMock, findTarget("Sprite1")).WillOnce(Return(4));
    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::createCloneByIndex)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::createCloneOfMyself)).WillOnce(Return(1));
    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::createClone)).WillOnce(Return(2));

    compiler.init();
    compiler.setBlock(block1);
    ControlBlocks::compileCreateClone(&compiler);
    compiler.setBlock(block2);
    ControlBlocks::compileCreateClone(&compiler);
    compiler.setBlock(block3);
    ControlBlocks::compileCreateClone(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 4);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, CreateCloneOfImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 2, vm::OP_HALT };
    static BlockFunc functions[] = { &ControlBlocks::createCloneByIndex, &ControlBlocks::createCloneOfMyself, &ControlBlocks::createClone };
    static Value constValues[] = { 4, "Sprite1", "_myself_" };

    Sprite sprite;
    sprite.setEngine(&m_engineMock);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, targetAt(4)).WillOnce(Return(&sprite));
    EXPECT_CALL(m_engineMock, initClone).Times(1);

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.allChildren().size(), 1);

    EXPECT_CALL(m_engineMock, initClone).Times(1);

    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.allChildren().size(), 2);
    ASSERT_EQ(sprite.children(), sprite.allChildren());

    EXPECT_CALL(m_engineMock, findTarget).WillOnce(Return(4));
    EXPECT_CALL(m_engineMock, targetAt(4)).WillOnce(Return(&sprite));
    EXPECT_CALL(m_engineMock, initClone).Times(1);

    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.allChildren().size(), 3);
    ASSERT_EQ(sprite.children(), sprite.allChildren());

    EXPECT_CALL(m_engineMock, initClone).Times(1);

    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.allChildren().size(), 4);
    ASSERT_EQ(sprite.children(), sprite.allChildren());
}

TEST_F(ControlBlocksTest, StartAsClone)
{
    Compiler compiler(&m_engineMock);

    auto block = createControlBlock("a", "control_start_as_clone");
    compiler.setBlock(block);

    EXPECT_CALL(m_engineMock, addCloneInitScript(block)).Times(1);
    ControlBlocks::compileStartAsClone(&compiler);
}

TEST_F(ControlBlocksTest, DeleteThisClone)
{
    Compiler compiler(&m_engineMock);

    auto block = createControlBlock("a", "control_delete_this_clone");

    EXPECT_CALL(m_engineMock, functionIndex(&ControlBlocks::deleteThisClone)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    ControlBlocks::compileDeleteThisClone(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(ControlBlocksTest, DeleteThisCloneImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &ControlBlocks::deleteThisClone };

    Sprite sprite;
    sprite.setEngine(&m_engineMock);

    Sprite *clone;
    EXPECT_CALL(m_engineMock, initClone(_)).WillOnce(SaveArg<0>(&clone));
    sprite.clone();
    ASSERT_TRUE(clone);

    VirtualMachine vm(clone, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, stopTarget(clone, nullptr)).Times(1);

    vm.setBytecode(bytecode);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.children().empty());
}
