#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/variable.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/variableblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

class VariableBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<VariableBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For set variable to and change variable by
        std::shared_ptr<Block> createVariableBlock(const std::string &id, const std::string &opcode, std::shared_ptr<Variable> variable, const Value &value) const
        {
            auto block = std::make_shared<Block>(id, opcode);

            auto variableField = std::make_shared<Field>("VARIABLE", Value(), variable);
            variableField->setFieldId(VariableBlocks::VARIABLE);
            block->addField(variableField);

            auto valueInput = std::make_shared<Input>("VALUE", Input::Type::Shadow);
            valueInput->setPrimaryValue(value);
            valueInput->setInputId(VariableBlocks::VALUE);
            block->addInput(valueInput);

            return block;
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(VariableBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Variables");
}

TEST_F(VariableBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(VariableBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_setvariableto", &VariableBlocks::compileSetVariable)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_changevariableby", &VariableBlocks::compileChangeVariableBy)).Times(1);

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "VALUE", VariableBlocks::VALUE));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "VARIABLE", VariableBlocks::VARIABLE));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(VariableBlocksTest, SetVariableTo)
{
    Compiler compiler(&m_engine);

    // set variable [var1] to "new value"
    auto var1 = std::make_shared<Variable>("b", "var1", "old value");
    auto block1 = createVariableBlock("a", "data_setvariableto", var1, "new value");

    // set variable [var2] to 5
    auto var2 = std::make_shared<Variable>("d", "var2", 2.5);
    auto block2 = createVariableBlock("c", "data_setvariableto", var2, 5);

    compiler.init();
    compiler.setBlock(block1);
    VariableBlocks::compileSetVariable(&compiler);
    compiler.setBlock(block2);
    VariableBlocks::compileSetVariable(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_SET_VAR, 0, vm::OP_CONST, 1, vm::OP_SET_VAR, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "new value", 5 }));
    ASSERT_EQ(
        compiler.variables(),
        std::vector<Variable *>({
            var1.get(),
            var2.get(),
        }));
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(VariableBlocksTest, ChangeVariableBy)
{
    Compiler compiler(&m_engine);

    // change variable [var1] by 10
    auto var1 = std::make_shared<Variable>("b", "var1", 2.5);
    auto block1 = createVariableBlock("a", "data_changevariableby", var1, 10);

    // change variable [var2] by 3.25
    auto var2 = std::make_shared<Variable>("d", "var2", 1.2);
    auto block2 = createVariableBlock("c", "data_changevariableby", var2, 3.25);

    compiler.init();
    compiler.setBlock(block1);
    VariableBlocks::compileChangeVariableBy(&compiler);
    compiler.setBlock(block2);
    VariableBlocks::compileChangeVariableBy(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_CHANGE_VAR, 0, vm::OP_CONST, 1, vm::OP_CHANGE_VAR, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 10, 3.25 }));
    ASSERT_EQ(
        compiler.variables(),
        std::vector<Variable *>({
            var1.get(),
            var2.get(),
        }));
    ASSERT_TRUE(compiler.lists().empty());
}
