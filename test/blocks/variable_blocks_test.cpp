#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/monitor.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/variableblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

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

        // For read variable
        std::shared_ptr<Block> createVariableBlock(const std::string &id, const std::string &opcode, std::shared_ptr<Variable> variable) const
        {
            auto block = std::make_shared<Block>(id, opcode);

            auto variableField = std::make_shared<Field>("VARIABLE", Value(), variable);
            variableField->setFieldId(VariableBlocks::VARIABLE);
            block->addField(variableField);

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
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_variable", &VariableBlocks::compileVariable)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_setvariableto", &VariableBlocks::compileSetVariable)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_changevariableby", &VariableBlocks::compileChangeVariableBy)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_showvariable", &VariableBlocks::compileShowVariable)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "data_hidevariable", &VariableBlocks::compileHideVariable)).Times(1);

    // Monitor names
    EXPECT_CALL(m_engineMock, addMonitorNameFunction(m_section.get(), "data_variable", &VariableBlocks::variableMonitorName));

    // Monitor change functions
    EXPECT_CALL(m_engineMock, addMonitorChangeFunction(m_section.get(), "data_variable", &VariableBlocks::changeVariableMonitorValue));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "VALUE", VariableBlocks::VALUE));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "VARIABLE", VariableBlocks::VARIABLE));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(VariableBlocksTest, Variable)
{
    Compiler compiler(&m_engine);

    // [var1]
    auto var1 = std::make_shared<Variable>("b", "var1");
    auto block1 = createVariableBlock("a", "data_variable", var1);

    // [var2]
    auto var2 = std::make_shared<Variable>("d", "var2");
    auto block2 = createVariableBlock("c", "data_variable", var2);

    compiler.init();
    compiler.setBlock(block1);
    VariableBlocks::compileVariable(&compiler);
    compiler.setBlock(block2);
    VariableBlocks::compileVariable(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_READ_VAR, 0, vm::OP_READ_VAR, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_EQ(
        compiler.variables(),
        std::vector<Variable *>({
            var1.get(),
            var2.get(),
        }));
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(VariableBlocksTest, VariableMonitorName)
{
    // [var1]
    auto var1 = std::make_shared<Variable>("b", "var1");
    auto block1 = createVariableBlock("a", "data_variable", var1);

    // [var2]
    auto var2 = std::make_shared<Variable>("d", "var2");
    auto block2 = createVariableBlock("c", "data_variable", var2);

    ASSERT_EQ(VariableBlocks::variableMonitorName(block1.get()), "var1");
    ASSERT_EQ(VariableBlocks::variableMonitorName(block2.get()), "var2");
}

TEST_F(VariableBlocksTest, ChangeVariableMonitorValue)
{
    // [var1]
    auto var1 = std::make_shared<Variable>("b", "var1", 2.5);
    auto block1 = createVariableBlock("a", "data_variable", var1);

    // [var2]
    auto var2 = std::make_shared<Variable>("d", "var2", "hello");
    auto block2 = createVariableBlock("c", "data_variable", var2);

    VariableBlocks::changeVariableMonitorValue(block1.get(), "test");
    ASSERT_EQ(var1->value().toString(), "test");
    ASSERT_EQ(var2->value().toString(), "hello");

    VariableBlocks::changeVariableMonitorValue(block2.get(), -0.25);
    ASSERT_EQ(var1->value().toString(), "test");
    ASSERT_EQ(var2->value().toDouble(), -0.25);
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

TEST_F(VariableBlocksTest, ShowVariable)
{
    Compiler compiler(&m_engineMock);
    Stage stage;
    Target target;

    // show variable [var1]
    auto var1 = std::make_shared<Variable>("b", "var1");
    var1->setTarget(&stage);
    auto block1 = createVariableBlock("a", "data_showvariable", var1);

    // show variable [var2]
    auto var2 = std::make_shared<Variable>("d", "var2");
    var2->setTarget(&target);
    auto block2 = createVariableBlock("c", "data_showvariable", var2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&VariableBlocks::showGlobalVariable)).WillOnce(Return(0));
    compiler.init();
    compiler.setBlock(block1);
    VariableBlocks::compileShowVariable(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&VariableBlocks::showVariable)).WillOnce(Return(1));
    compiler.setBlock(block2);
    VariableBlocks::compileShowVariable(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "b", "d" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(VariableBlocksTest, ShowVariableImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &VariableBlocks::showGlobalVariable, &VariableBlocks::showVariable };
    static Value constValues[] = { "a", "b", "c", "d" };

    auto var1 = std::make_shared<Variable>("b", "");
    Monitor monitor1("b", "");
    monitor1.setVisible(false);
    var1->setMonitor(&monitor1);

    auto var2 = std::make_shared<Variable>("d", "");
    Monitor monitor2("d", "");
    monitor2.setVisible(false);
    var2->setMonitor(&monitor2);

    Stage stage;
    stage.addVariable(var1);

    Target target;
    target.addVariable(var2);

    // Global
    VirtualMachine vm1(&stage, &m_engineMock, nullptr);
    vm1.setBytecode(bytecode1);
    vm1.setFunctions(functions);
    vm1.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.reset();
    vm1.setBytecode(bytecode2);
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    monitor1.setVisible(false);

    // Local
    VirtualMachine vm2(&target, &m_engineMock, nullptr);
    vm2.setBytecode(bytecode3);
    vm2.setFunctions(functions);
    vm2.setConstValues(constValues);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());

    vm2.reset();
    vm2.setBytecode(bytecode4);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());
}

TEST_F(VariableBlocksTest, HideVariable)
{
    Compiler compiler(&m_engineMock);
    Stage stage;
    Target target;

    // hide variable [var1]
    auto var1 = std::make_shared<Variable>("b", "var1");
    var1->setTarget(&stage);
    auto block1 = createVariableBlock("a", "data_hidevariable", var1);

    // hide variable [var2]
    auto var2 = std::make_shared<Variable>("d", "var2");
    var2->setTarget(&target);
    auto block2 = createVariableBlock("c", "data_hidevariable", var2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&VariableBlocks::hideGlobalVariable)).WillOnce(Return(0));
    compiler.init();
    compiler.setBlock(block1);
    VariableBlocks::compileHideVariable(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&VariableBlocks::hideVariable)).WillOnce(Return(1));
    compiler.setBlock(block2);
    VariableBlocks::compileHideVariable(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "b", "d" }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(VariableBlocksTest, HideVariableImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &VariableBlocks::hideGlobalVariable, &VariableBlocks::hideVariable };
    static Value constValues[] = { "a", "b", "c", "d" };

    auto var1 = std::make_shared<Variable>("b", "");
    Monitor monitor1("b", "");
    monitor1.setVisible(true);
    var1->setMonitor(&monitor1);

    auto var2 = std::make_shared<Variable>("d", "");
    Monitor monitor2("d", "");
    monitor2.setVisible(true);
    var2->setMonitor(&monitor2);

    Stage stage;
    stage.addVariable(var1);

    Target target;
    target.addVariable(var2);

    // Global
    VirtualMachine vm1(&stage, &m_engineMock, nullptr);
    vm1.setBytecode(bytecode1);
    vm1.setFunctions(functions);
    vm1.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm1.reset();
    vm1.setBytecode(bytecode2);
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);
    ASSERT_FALSE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    monitor1.setVisible(true);

    // Local
    VirtualMachine vm2(&target, &m_engineMock, nullptr);
    vm2.setBytecode(bytecode3);
    vm2.setFunctions(functions);
    vm2.setConstValues(constValues);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_TRUE(monitor2.visible());

    vm2.reset();
    vm2.setBytecode(bytecode4);
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
    ASSERT_TRUE(monitor1.visible());
    ASSERT_FALSE(monitor2.visible());
}
