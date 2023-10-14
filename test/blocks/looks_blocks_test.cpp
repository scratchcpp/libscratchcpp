#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <enginemock.h>
#include <randomgeneratormock.h>

#include "../common.h"
#include "blocks/looksblocks.h"
#include "blocks/operatorblocks.h"
#include "engine/internal/engine.h"
#include "engine/internal/randomgenerator.h"

using namespace libscratchcpp;

using ::testing::Return;

class LooksBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<LooksBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        // For any looks block
        std::shared_ptr<Block> createLooksBlock(const std::string &id, const std::string &opcode) const { return std::make_shared<Block>(id, opcode); }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = createLooksBlock(block->id() + "_menu", block->opcode() + "_menu");
                input->setValueBlock(menu);
                addDropdownField(menu, name, static_cast<LooksBlocks::Fields>(-1), selectedValue, static_cast<LooksBlocks::FieldValues>(-1));
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, LooksBlocks::Fields id, const std::string &value, LooksBlocks::FieldValues valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
            block->updateFieldMap();
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
};

TEST_F(LooksBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Looks");
}

TEST_F(LooksBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(LooksBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_show", &LooksBlocks::compileShow));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_hide", &LooksBlocks::compileHide));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_changesizeby", &LooksBlocks::compileChangeSizeBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_setsizeto", &LooksBlocks::compileSetSizeTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_size", &LooksBlocks::compileSize));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_switchcostumeto", &LooksBlocks::compileSwitchCostumeTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_nextcostume", &LooksBlocks::compileNextCostume));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_switchbackdropto", &LooksBlocks::compileSwitchBackdropTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_switchbackdroptoandwait", &LooksBlocks::compileSwitchBackdropToAndWait));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_nextbackdrop", &LooksBlocks::compileNextBackdrop));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_costumenumbername", &LooksBlocks::compileCostumeNumberName));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "looks_backdropnumbername", &LooksBlocks::compileBackdropNumberName));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "CHANGE", LooksBlocks::CHANGE));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SIZE", LooksBlocks::SIZE));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "COSTUME", LooksBlocks::COSTUME));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "BACKDROP", LooksBlocks::BACKDROP));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "NUMBER_NAME", LooksBlocks::NUMBER_NAME));

    // Field values
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "number", LooksBlocks::Number));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "name", LooksBlocks::Name));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(LooksBlocksTest, Show)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_show");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::show)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileShow(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, ShowImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::show };

    Sprite sprite;
    sprite.setVisible(false);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.visible());

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.visible());
}

TEST_F(LooksBlocksTest, Hide)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_hide");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::hide)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileHide(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, HideImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::hide };

    Sprite sprite;
    sprite.setVisible(true);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(sprite.visible());

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(sprite.visible());
}

TEST_F(LooksBlocksTest, ChangeSizeBy)
{
    Compiler compiler(&m_engineMock);

    // change size by (10.05)
    auto block = std::make_shared<Block>("a", "looks_changesizeby");
    addValueInput(block, "CHANGE", LooksBlocks::CHANGE, 10.05);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::changeSizeBy)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileChangeSizeBy(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 10.05);
}

TEST_F(LooksBlocksTest, ChangeSizeByImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::changeSizeBy };
    static Value constValues[] = { 10.05 };

    Sprite sprite;
    sprite.setSize(1.308);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.size(), 11.358);
}

TEST_F(LooksBlocksTest, SetSizeTo)
{
    Compiler compiler(&m_engineMock);

    // set size to (87.654)
    auto block = std::make_shared<Block>("a", "looks_setsizeto");
    addValueInput(block, "SIZE", LooksBlocks::SIZE, 87.654);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::setSizeTo)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileSetSizeTo(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 87.654);
}

TEST_F(LooksBlocksTest, SetSizeToImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::setSizeTo };
    static Value constValues[] = { 87.654 };

    Sprite sprite;
    sprite.setSize(5.684);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(sprite.size(), 87.654);
}

TEST_F(LooksBlocksTest, Size)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_size");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::size)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileSize(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, SizeImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::size };

    Sprite sprite;
    sprite.setSize(-51.0684);

    VirtualMachine vm(&sprite, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -51.0684);
}

TEST_F(LooksBlocksTest, SwitchCostumeTo)
{
    Target target;
    Compiler compiler(&m_engineMock, &target);

    // switch costume to (costume2)
    auto block1 = std::make_shared<Block>("a", "looks_switchcostumeto");
    addDropdownInput(block1, "COSTUME", LooksBlocks::COSTUME, "costume2");

    // switch costume to (0)
    auto block2 = std::make_shared<Block>("b", "looks_switchcostumeto");
    addDropdownInput(block2, "COSTUME", LooksBlocks::COSTUME, "0");

    // switch costume to (1)
    auto block3 = std::make_shared<Block>("b", "looks_switchcostumeto");
    addDropdownInput(block3, "COSTUME", LooksBlocks::COSTUME, "1");

    // switch costume to (2)
    auto block4 = std::make_shared<Block>("b", "looks_switchcostumeto");
    addDropdownInput(block4, "COSTUME", LooksBlocks::COSTUME, "2");

    // switch costume to (4)
    auto block5 = std::make_shared<Block>("b", "looks_switchcostumeto");
    addDropdownInput(block5, "COSTUME", LooksBlocks::COSTUME, "4");

    // switch costume to (3) - there's a costume with this name
    auto block6 = std::make_shared<Block>("c", "looks_switchcostumeto");
    addDropdownInput(block6, "COSTUME", LooksBlocks::COSTUME, "3");

    // switch costume to (next costume)
    auto block7 = std::make_shared<Block>("d", "looks_switchcostumeto");
    addDropdownInput(block7, "COSTUME", LooksBlocks::COSTUME, "next costume");

    // switch costume to (next costume) - there's a costume with this name
    auto block8 = std::make_shared<Block>("d", "looks_switchcostumeto");
    addDropdownInput(block8, "COSTUME", LooksBlocks::COSTUME, "next costume");

    // switch costume to (previous costume)
    auto block9 = std::make_shared<Block>("e", "looks_switchcostumeto");
    addDropdownInput(block9, "COSTUME", LooksBlocks::COSTUME, "previous costume");

    // switch costume to (previous costume) - there's a costume with this name
    auto block10 = std::make_shared<Block>("f", "looks_switchcostumeto");
    addDropdownInput(block10, "COSTUME", LooksBlocks::COSTUME, "previous costume");

    // switch costume to (join "" "")
    auto joinBlock = std::make_shared<Block>("h", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block11 = std::make_shared<Block>("g", "looks_switchcostumeto");
    addDropdownInput(block11, "COSTUME", LooksBlocks::COSTUME, "", joinBlock);

    compiler.init();

    // Test without any costumes first
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    target.addCostume(std::make_shared<Costume>("costume1", "c1", "svg"));
    target.addCostume(std::make_shared<Costume>("costume2", "c2", "svg"));

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    target.addCostume(std::make_shared<Costume>("costume3", "c3", "svg"));

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block2);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block3);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block4);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block5);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    target.addCostume(std::make_shared<Costume>("3", "c4", "svg"));

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block6);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::nextCostume)).WillOnce(Return(1));
    compiler.setBlock(block7);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    target.addCostume(std::make_shared<Costume>("next costume", "c5", "svg"));

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block8);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::previousCostume)).WillOnce(Return(2));
    compiler.setBlock(block9);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    target.addCostume(std::make_shared<Costume>("previous costume", "c6", "svg"));

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block10);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchCostumeTo)).WillOnce(Return(3));
    compiler.setBlock(block11);
    LooksBlocks::compileSwitchCostumeTo(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
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
              vm::OP_EXEC,
              1,
              vm::OP_CONST,
              6,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              2,
              vm::OP_CONST,
              7,
              vm::OP_EXEC,
              0,
              vm::OP_NULL,
              vm::OP_NULL,
              vm::OP_STR_CONCAT,
              vm::OP_EXEC,
              3,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 1, -1, 0, 1, 3, 3, 4, 5 }));
}

TEST_F(LooksBlocksTest, SwitchCostumeToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::switchCostumeTo, &LooksBlocks::switchCostumeToByIndex };
    static Value constValues[] = { "costume2", 0, 1, 2, 3, "next costume", "previous costume", -1, 0, 5, 6 };

    Target target;
    target.addCostume(std::make_shared<Costume>("costume1", "c1", "svg"));
    target.addCostume(std::make_shared<Costume>("costume2", "c2", "svg"));
    target.setCurrentCostume(1);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    // "costume2"
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 2);

    // 0
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 2);

    target.setCurrentCostume(1);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 2);

    // 1
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 1);

    // 2
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 2);

    // 3
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 1);

    target.setCurrentCostume(2);

    // "2"
    target.addCostume(std::make_shared<Costume>("2", "c3", "svg"));
    target.addCostume(std::make_shared<Costume>("test", "c4", "svg"));
    target.setCurrentCostume(1);

    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 3);

    // "next costume"
    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 4);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 1);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 2);

    target.addCostume(std::make_shared<Costume>("next costume", "c5", "svg"));

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 5);

    // "previous costume"
    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 4);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 3);

    target.setCurrentCostume(1);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 5);

    target.addCostume(std::make_shared<Costume>("previous costume", "c6", "svg"));

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 6);

    // -1 (index)
    target.setCurrentCostume(2);

    vm.setBytecode(bytecode9);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 6);

    // 0 (index)
    vm.setBytecode(bytecode10);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 1);

    // 5 (index)
    vm.setBytecode(bytecode11);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 6);

    // 6 (index)
    vm.setBytecode(bytecode12);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 1);
}

TEST_F(LooksBlocksTest, NextCostume)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_nextcostume");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::nextCostume)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileNextCostume(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, NextCostumeImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::nextCostume };

    Target target;
    target.addCostume(std::make_shared<Costume>("costume1", "c1", "svg"));
    target.addCostume(std::make_shared<Costume>("costume2", "c2", "svg"));
    target.addCostume(std::make_shared<Costume>("costume3", "c3", "svg"));
    target.setCurrentCostume(1);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 2);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 3);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 1);
}

TEST_F(LooksBlocksTest, PreviousCostume)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::previousCostume };

    Target target;
    target.addCostume(std::make_shared<Costume>("costume1", "c1", "svg"));
    target.addCostume(std::make_shared<Costume>("costume2", "c2", "svg"));
    target.addCostume(std::make_shared<Costume>("costume3", "c3", "svg"));
    target.setCurrentCostume(3);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 2);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 1);

    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(target.currentCostume(), 3);
}

TEST_F(LooksBlocksTest, SwitchBackdropTo)
{
    Target target;
    Stage stage;
    Compiler compiler(&m_engineMock, &target);

    // switch backdrop to (backdrop2)
    auto block1 = std::make_shared<Block>("a", "looks_switchbackdropto");
    addDropdownInput(block1, "BACKDROP", LooksBlocks::BACKDROP, "backdrop2");

    // switch backdrop to (0)
    auto block2 = std::make_shared<Block>("b", "looks_switchbackdropto");
    addDropdownInput(block2, "BACKDROP", LooksBlocks::BACKDROP, "0");

    // switch backdrop to (1)
    auto block3 = std::make_shared<Block>("b", "looks_switchbackdropto");
    addDropdownInput(block3, "BACKDROP", LooksBlocks::BACKDROP, "1");

    // switch backdrop to (2)
    auto block4 = std::make_shared<Block>("b", "looks_switchbackdropto");
    addDropdownInput(block4, "BACKDROP", LooksBlocks::BACKDROP, "2");

    // switch backdrop to (4)
    auto block5 = std::make_shared<Block>("b", "looks_switchbackdropto");
    addDropdownInput(block5, "BACKDROP", LooksBlocks::BACKDROP, "4");

    // switch backdrop to (3) - there's a backdrop with this name
    auto block6 = std::make_shared<Block>("c", "looks_switchbackdropto");
    addDropdownInput(block6, "BACKDROP", LooksBlocks::BACKDROP, "3");

    // switch backdrop to (next backdrop)
    auto block7 = std::make_shared<Block>("d", "looks_switchbackdropto");
    addDropdownInput(block7, "BACKDROP", LooksBlocks::BACKDROP, "next backdrop");

    // switch backdrop to (next backdrop) - there's a backdrop with this name
    auto block8 = std::make_shared<Block>("d", "looks_switchbackdropto");
    addDropdownInput(block8, "BACKDROP", LooksBlocks::BACKDROP, "next backdrop");

    // switch backdrop to (previous backdrop)
    auto block9 = std::make_shared<Block>("e", "looks_switchbackdropto");
    addDropdownInput(block9, "BACKDROP", LooksBlocks::BACKDROP, "previous backdrop");

    // switch backdrop to (previous backdrop) - there's a backdrop with this name
    auto block10 = std::make_shared<Block>("f", "looks_switchbackdropto");
    addDropdownInput(block10, "BACKDROP", LooksBlocks::BACKDROP, "previous backdrop");

    // switch backdrop to (random backdrop)
    auto block11 = std::make_shared<Block>("g", "looks_switchbackdropto");
    addDropdownInput(block11, "BACKDROP", LooksBlocks::BACKDROP, "random backdrop");

    // switch backdrop to (random backdrop) - there's a backdrop with this name
    auto block12 = std::make_shared<Block>("h", "looks_switchbackdropto");
    addDropdownInput(block12, "BACKDROP", LooksBlocks::BACKDROP, "random backdrop");

    // switch backdrop to (join "" "")
    auto joinBlock = std::make_shared<Block>("j", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block13 = std::make_shared<Block>("i", "looks_switchbackdropto");
    addDropdownInput(block13, "BACKDROP", LooksBlocks::BACKDROP, "", joinBlock);

    compiler.init();

    // Test without any backdrops first
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block2);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block3);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block4);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block5);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    stage.addCostume(std::make_shared<Costume>("3", "b4", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block6);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::nextBackdrop)).WillOnce(Return(1));
    compiler.setBlock(block7);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    stage.addCostume(std::make_shared<Costume>("next backdrop", "b5", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block8);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::previousBackdrop)).WillOnce(Return(2));
    compiler.setBlock(block9);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    stage.addCostume(std::make_shared<Costume>("previous backdrop", "b6", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block10);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::randomBackdrop)).WillOnce(Return(3));
    compiler.setBlock(block11);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    stage.addCostume(std::make_shared<Costume>("random backdrop", "b7", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block12);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropTo)).WillOnce(Return(4));
    compiler.setBlock(block13);
    LooksBlocks::compileSwitchBackdropTo(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
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
              vm::OP_EXEC,
              1,
              vm::OP_CONST,
              6,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              2,
              vm::OP_CONST,
              7,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              3,
              vm::OP_CONST,
              8,
              vm::OP_EXEC,
              0,
              vm::OP_NULL,
              vm::OP_NULL,
              vm::OP_STR_CONCAT,
              vm::OP_EXEC,
              4,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 1, -1, 0, 1, 3, 3, 4, 5, 6 }));
}

TEST_F(LooksBlocksTest, SwitchBackdropToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::switchBackdropTo, &LooksBlocks::switchBackdropToByIndex };
    static Value constValues[] = { "backdrop2", 0, 1, 2, 3, "next backdrop", "previous backdrop", -1, 0, 5, 6 };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.setCurrentCostume(1);

    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    // "backdrop2"
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    // 0
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    stage.setCurrentCostume(1);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    // 1
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    // 2
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    // 3
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    stage.setCurrentCostume(2);

    // "2"
    stage.addCostume(std::make_shared<Costume>("2", "b3", "svg"));
    stage.addCostume(std::make_shared<Costume>("test", "b4", "svg"));
    stage.setCurrentCostume(1);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 3);

    // "next backdrop"
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 4);

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    stage.addCostume(std::make_shared<Costume>("next backdrop", "b5", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 5);

    // "previous backdrop"
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 4);

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 3);

    stage.setCurrentCostume(1);

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 5);

    stage.addCostume(std::make_shared<Costume>("previous backdrop", "b6", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 6);

    // -1 (index)
    stage.setCurrentCostume(2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode9);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 6);

    // 0 (index)
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode10);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    // 5 (index)
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode11);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 6);

    // 6 (index)
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode12);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);
}

TEST_F(LooksBlocksTest, SwitchBackdropToAndWait)
{
    Target target;
    Stage stage;
    Compiler compiler(&m_engineMock, &target);

    // switch backdrop to (backdrop2) and wait
    auto block1 = std::make_shared<Block>("a", "looks_switchbackdroptoandwait");
    addDropdownInput(block1, "BACKDROP", LooksBlocks::BACKDROP, "backdrop2");

    // switch backdrop to (0) and wait
    auto block2 = std::make_shared<Block>("b", "looks_switchbackdroptoandwait");
    addDropdownInput(block2, "BACKDROP", LooksBlocks::BACKDROP, "0");

    // switch backdrop to (1) and wait
    auto block3 = std::make_shared<Block>("b", "looks_switchbackdroptoandwait");
    addDropdownInput(block3, "BACKDROP", LooksBlocks::BACKDROP, "1");

    // switch backdrop to (2) and wait
    auto block4 = std::make_shared<Block>("b", "looks_switchbackdroptoandwait");
    addDropdownInput(block4, "BACKDROP", LooksBlocks::BACKDROP, "2");

    // switch backdrop to (4) and wait
    auto block5 = std::make_shared<Block>("b", "looks_switchbackdroptoandwait");
    addDropdownInput(block5, "BACKDROP", LooksBlocks::BACKDROP, "4");

    // switch backdrop to (3) and wait - there's a backdrop with this name
    auto block6 = std::make_shared<Block>("c", "looks_switchbackdroptoandwait");
    addDropdownInput(block6, "BACKDROP", LooksBlocks::BACKDROP, "3");

    // switch backdrop to (next backdrop) and wait
    auto block7 = std::make_shared<Block>("d", "looks_switchbackdroptoandwait");
    addDropdownInput(block7, "BACKDROP", LooksBlocks::BACKDROP, "next backdrop");

    // switch backdrop to (next backdrop) and wait - there's a backdrop with this name
    auto block8 = std::make_shared<Block>("d", "looks_switchbackdroptoandwait");
    addDropdownInput(block8, "BACKDROP", LooksBlocks::BACKDROP, "next backdrop");

    // switch backdrop to (previous backdrop) and wait
    auto block9 = std::make_shared<Block>("e", "looks_switchbackdroptoandwait");
    addDropdownInput(block9, "BACKDROP", LooksBlocks::BACKDROP, "previous backdrop");

    // switch backdrop to (previous backdrop) and wait - there's a backdrop with this name
    auto block10 = std::make_shared<Block>("f", "looks_switchbackdroptoandwait");
    addDropdownInput(block10, "BACKDROP", LooksBlocks::BACKDROP, "previous backdrop");

    // switch backdrop to (random backdrop) and wait
    auto block11 = std::make_shared<Block>("g", "looks_switchbackdroptoandwait");
    addDropdownInput(block11, "BACKDROP", LooksBlocks::BACKDROP, "random backdrop");

    // switch backdrop to (random backdrop) and wait - there's a backdrop with this name
    auto block12 = std::make_shared<Block>("h", "looks_switchbackdroptoandwait");
    addDropdownInput(block12, "BACKDROP", LooksBlocks::BACKDROP, "random backdrop");

    // switch backdrop to (join "" "") and wait
    auto joinBlock = std::make_shared<Block>("j", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block13 = std::make_shared<Block>("i", "looks_switchbackdroptoandwait");
    addDropdownInput(block13, "BACKDROP", LooksBlocks::BACKDROP, "", joinBlock);

    compiler.init();

    // Test without any backdrops first
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block1);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block2);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block3);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block4);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block5);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    stage.addCostume(std::make_shared<Costume>("3", "b4", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block6);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::nextBackdropAndWait)).WillOnce(Return(1));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block7);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    stage.addCostume(std::make_shared<Costume>("next backdrop", "b5", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block8);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::previousBackdropAndWait)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block9);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    stage.addCostume(std::make_shared<Costume>("previous backdrop", "b6", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block10);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::randomBackdropAndWait)).WillOnce(Return(3));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block11);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    stage.addCostume(std::make_shared<Costume>("random backdrop", "b7", "svg"));

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToByIndexAndWait)).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block12);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::switchBackdropToAndWait)).WillOnce(Return(4));
    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::checkBackdropScripts)).WillOnce(Return(5));
    compiler.setBlock(block13);
    LooksBlocks::compileSwitchBackdropToAndWait(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              0,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              2,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              3,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              4,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              5,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_EXEC,
              1,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              6,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_EXEC,
              2,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              7,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_EXEC,
              3,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              8,
              vm::OP_EXEC,
              0,
              vm::OP_EXEC,
              5,
              vm::OP_NULL,
              vm::OP_NULL,
              vm::OP_STR_CONCAT,
              vm::OP_EXEC,
              4,
              vm::OP_EXEC,
              5,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 1, -1, 0, 1, 3, 3, 4, 5, 6 }));
}

TEST_F(LooksBlocksTest, SwitchBackdropToAndWaitImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 8, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 9, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 10, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::switchBackdropToAndWait, &LooksBlocks::switchBackdropToByIndexAndWait };
    static Value constValues[] = { "backdrop2", 0, 1, 2, 3, "next backdrop", "previous backdrop", -1, 0, 5, 6 };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.setCurrentCostume(1);

    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    // "backdrop2"
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(1)->broadcast(), &vm, true));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    // 0
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(1)->broadcast(), &vm, true));
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    stage.setCurrentCostume(1);

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(1)->broadcast(), &vm, true));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    // 1
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(0)->broadcast(), &vm, true));
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    // 2
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(1)->broadcast(), &vm, true));
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    // 3
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(0)->broadcast(), &vm, true));
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    stage.setCurrentCostume(2);

    // "2"
    stage.addCostume(std::make_shared<Costume>("2", "b3", "svg"));
    stage.addCostume(std::make_shared<Costume>("test", "b4", "svg"));
    stage.setCurrentCostume(1);

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(2)->broadcast(), &vm, true));
    vm.setBytecode(bytecode6);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 3);

    // "next backdrop"
    EXPECT_CALL(m_engineMock, stage()).Times(3).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(3)->broadcast(), &vm, true));
    vm.setBytecode(bytecode7);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 4);

    EXPECT_CALL(m_engineMock, stage()).Times(3).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(0)->broadcast(), &vm, true));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    EXPECT_CALL(m_engineMock, stage()).Times(3).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(1)->broadcast(), &vm, true));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    stage.addCostume(std::make_shared<Costume>("next backdrop", "b5", "svg"));

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(4)->broadcast(), &vm, true));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 5);

    // "previous backdrop"
    EXPECT_CALL(m_engineMock, stage()).Times(3).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(3)->broadcast(), &vm, true));
    vm.setBytecode(bytecode8);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 4);

    EXPECT_CALL(m_engineMock, stage()).Times(3).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(2)->broadcast(), &vm, true));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 3);

    stage.setCurrentCostume(1);

    EXPECT_CALL(m_engineMock, stage()).Times(3).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(4)->broadcast(), &vm, true));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 5);

    stage.addCostume(std::make_shared<Costume>("previous backdrop", "b6", "svg"));

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(5)->broadcast(), &vm, true));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 6);

    // -1 (index)
    stage.setCurrentCostume(2);

    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(5)->broadcast(), &vm, true));
    vm.setBytecode(bytecode9);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 6);

    // 0 (index)
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(0)->broadcast(), &vm, true));
    vm.setBytecode(bytecode10);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    // 5 (index)
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(5)->broadcast(), &vm, true));
    vm.setBytecode(bytecode11);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 6);

    // 6 (index)
    EXPECT_CALL(m_engineMock, stage()).Times(2).WillRepeatedly(Return(&stage));
    EXPECT_CALL(m_engineMock, broadcastByPtr(stage.costumeAt(0)->broadcast(), &vm, true));
    vm.setBytecode(bytecode12);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);
}

TEST_F(LooksBlocksTest, NextBackdrop)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "looks_nextbackdrop");

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::nextBackdrop)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    LooksBlocks::compileNextBackdrop(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, NextBackdropImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::nextBackdrop };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));
    stage.setCurrentCostume(1);

    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 3);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);
}

TEST_F(LooksBlocksTest, PreviousBackdrop)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::previousBackdrop };

    Target target;

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));
    stage.setCurrentCostume(3);

    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 1);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 3);
}

TEST_F(LooksBlocksTest, RandomBackdrop)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::randomBackdrop };

    Target target;

    Stage stage;

    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);

    RandomGeneratorMock rng;
    LooksBlocks::rng = &rng;

    EXPECT_CALL(rng, randint).Times(0);
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);

    stage.addCostume(std::make_shared<Costume>("backdrop1", "b1", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b2", "svg"));
    stage.addCostume(std::make_shared<Costume>("backdrop3", "b3", "svg"));

    EXPECT_CALL(rng, randint(1, 3)).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 2);

    stage.addCostume(std::make_shared<Costume>("backdrop4", "b4", "svg"));

    EXPECT_CALL(rng, randint(1, 4)).WillOnce(Return(3));
    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(stage.currentCostume(), 3);

    LooksBlocks::rng = RandomGenerator::instance().get();
}

TEST_F(LooksBlocksTest, CostumeNumberName)
{
    Compiler compiler(&m_engineMock);

    // costume [number]
    auto block1 = std::make_shared<Block>("a", "looks_costumenumbername");
    addDropdownField(block1, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "number", LooksBlocks::Number);

    // costume [name]
    auto block2 = std::make_shared<Block>("b", "looks_costumenumbername");
    addDropdownField(block2, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "name", LooksBlocks::Name);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::costumeNumber)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileCostumeNumberName(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::costumeName)).WillOnce(Return(1));
    compiler.setBlock(block2);
    LooksBlocks::compileCostumeNumberName(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, CostumeNumberNameImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::costumeNumber, &LooksBlocks::costumeName };

    auto c1 = std::make_shared<Costume>("costume1", "c1", "svg");
    auto c2 = std::make_shared<Costume>("costume2", "c2", "svg");
    auto c3 = std::make_shared<Costume>("costume3", "c3", "svg");

    Target target;
    target.addCostume(c1);
    target.addCostume(c2);
    target.addCostume(c3);

    VirtualMachine vm(&target, nullptr, nullptr);
    vm.setFunctions(functions);

    target.setCurrentCostume(2);

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "costume2");

    target.setCurrentCostume(3);

    vm.reset();
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 3);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "costume3");
}

TEST_F(LooksBlocksTest, BackdropNumberName)
{
    Compiler compiler(&m_engineMock);

    // backdrop [number]
    auto block1 = std::make_shared<Block>("a", "looks_backdropnumbername");
    addDropdownField(block1, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "number", LooksBlocks::Number);

    // backdrop [name]
    auto block2 = std::make_shared<Block>("b", "looks_backdropnumbername");
    addDropdownField(block2, "NUMBER_NAME", LooksBlocks::NUMBER_NAME, "name", LooksBlocks::Name);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::backdropNumber)).WillOnce(Return(0));
    compiler.setBlock(block1);
    LooksBlocks::compileBackdropNumberName(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&LooksBlocks::backdropName)).WillOnce(Return(1));
    compiler.setBlock(block2);
    LooksBlocks::compileBackdropNumberName(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(LooksBlocksTest, BackdropNumberNameImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &LooksBlocks::backdropNumber, &LooksBlocks::backdropName };

    auto b1 = std::make_shared<Costume>("backdrop1", "b1", "svg");
    auto b2 = std::make_shared<Costume>("backdrop2", "b2", "svg");
    auto b3 = std::make_shared<Costume>("backdrop3", "b3", "svg");

    Target target;

    Stage stage;
    stage.addCostume(b1);
    stage.addCostume(b2);
    stage.addCostume(b3);

    VirtualMachine vm(&target, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    stage.setCurrentCostume(2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "backdrop2");

    stage.setCurrentCostume(3);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 3);

    EXPECT_CALL(m_engineMock, stage()).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "backdrop3");
}
