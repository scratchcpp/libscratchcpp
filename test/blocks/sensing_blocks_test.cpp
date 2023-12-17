#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/variable.h>
#include <enginemock.h>
#include <timermock.h>
#include <clockmock.h>

#include "../common.h"
#include "blocks/sensingblocks.h"
#include "blocks/operatorblocks.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

using ::testing::Return;

class SensingBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_section = std::make_unique<SensingBlocks>();
            m_section->registerBlocks(&m_engine);
        }

        std::shared_ptr<Block> createSensingCurrentBlock(const std::string &id, const std::string &optionStr, SensingBlocks::FieldValues option)
        {
            auto block = std::make_shared<Block>(id, "sensing_current");
            auto optionField = std::make_shared<Field>("CURRENTMENU", optionStr);
            optionField->setFieldId(SensingBlocks::CURRENTMENU);
            optionField->setSpecialValueId(option);
            block->addField(optionField);

            return block;
        }

        std::shared_ptr<Block> createNullBlock(const std::string &id)
        {
            std::shared_ptr<Block> block = std::make_shared<Block>(id, "");
            BlockComp func = [](Compiler *compiler) { compiler->addInstruction(vm::OP_NULL); };
            block->setCompileFunction(func);

            return block;
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, SensingBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, SensingBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, SensingBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = std::make_shared<Block>(block->id() + "_menu", block->opcode() + "_menu");
                input->setValueBlock(menu);
                addDropdownField(menu, name, static_cast<SensingBlocks::Fields>(-1), selectedValue, static_cast<SensingBlocks::FieldValues>(-1));
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, SensingBlocks::Fields id, const std::string &value, SensingBlocks::FieldValues valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
        Engine m_engine;
        TimerMock m_timerMock;
        ClockMock m_clockMock;
};

TEST_F(SensingBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Sensing");
}

TEST_F(SensingBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(SensingBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_distanceto", &SensingBlocks::compileDistanceTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_keypressed", &SensingBlocks::compileKeyPressed));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_mousedown", &SensingBlocks::compileMouseDown));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_mousex", &SensingBlocks::compileMouseX));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_mousey", &SensingBlocks::compileMouseY));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_setdragmode", &SensingBlocks::compileSetDragMode));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_timer", &SensingBlocks::compileTimer));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_resettimer", &SensingBlocks::compileResetTimer));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_of", &SensingBlocks::compileOf));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_current", &SensingBlocks::compileCurrent));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_dayssince2000", &SensingBlocks::compileDaysSince2000));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DISTANCETOMENU", SensingBlocks::DISTANCETOMENU));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "KEY_OPTION", SensingBlocks::KEY_OPTION));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "OBJECT", SensingBlocks::OBJECT));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "CURRENTMENU", SensingBlocks::CURRENTMENU));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "DRAG_MODE", SensingBlocks::DRAG_MODE));
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "PROPERTY", SensingBlocks::PROPERTY));

    // Field values
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "YEAR", SensingBlocks::YEAR));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "MONTH", SensingBlocks::MONTH));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "DATE", SensingBlocks::DATE));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "DAYOFWEEK", SensingBlocks::DAYOFWEEK));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "HOUR", SensingBlocks::HOUR));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "MINUTE", SensingBlocks::MINUTE));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "SECOND", SensingBlocks::SECOND));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "draggable", SensingBlocks::Draggable));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "not draggable", SensingBlocks::NotDraggable));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "x position", SensingBlocks::XPosition));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "y position", SensingBlocks::YPosition));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "direction", SensingBlocks::Direction));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "costume #", SensingBlocks::CostumeNumber));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "costume name", SensingBlocks::CostumeName));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "size", SensingBlocks::Size));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "volume", SensingBlocks::Volume));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "background #", SensingBlocks::BackdropNumber));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "backdrop #", SensingBlocks::BackdropNumber));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "backdrop name", SensingBlocks::BackdropName));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(SensingBlocksTest, DistanceTo)
{
    Compiler compiler(&m_engineMock);

    // distance to (Sprite2)
    auto block1 = std::make_shared<Block>("a", "sensing_distanceto");
    addDropdownInput(block1, "DISTANCETOMENU", SensingBlocks::DISTANCETOMENU, "Sprite2");

    // distance to (mouse-pointer)
    auto block2 = std::make_shared<Block>("b", "sensing_distanceto");
    addDropdownInput(block2, "DISTANCETOMENU", SensingBlocks::DISTANCETOMENU, "_mouse_");

    // distance to (null block)
    auto block3 = std::make_shared<Block>("c", "sensing_distanceto");
    addDropdownInput(block3, "DISTANCETOMENU", SensingBlocks::DISTANCETOMENU, "", createNullBlock("d"));

    compiler.init();

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(5));
    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::distanceToByIndex)).WillOnce(Return(0));
    compiler.setBlock(block1);
    SensingBlocks::compileDistanceTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::distanceToMousePointer)).WillOnce(Return(1));
    compiler.setBlock(block2);
    SensingBlocks::compileDistanceTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::distanceTo)).WillOnce(Return(2));
    compiler.setBlock(block3);
    SensingBlocks::compileDistanceTo(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 5);
}

TEST_F(SensingBlocksTest, DistanceToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::distanceTo, &SensingBlocks::distanceToByIndex, &SensingBlocks::distanceToMousePointer };
    static Value constValues[] = { "Sprite2", "_mouse_", "", 0, 1, -1, 2 };

    Sprite sprite1;
    sprite1.setX(-50.35);
    sprite1.setY(33.04);

    Sprite sprite2;
    sprite2.setX(108.564);
    sprite2.setY(-168.452);

    VirtualMachine vm(&sprite1, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(3));
    EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&sprite2));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(std::round(vm.getInput(0, 1)->toDouble() * 10000) / 10000, 256.6178);

    EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(-239.98));
    EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-86.188));
    vm.setBytecode(bytecode2);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(std::round(vm.getInput(0, 1)->toDouble() * 10000) / 10000, 223.9974);

    EXPECT_CALL(m_engineMock, findTarget("")).WillOnce(Return(-1));
    EXPECT_CALL(m_engineMock, targetAt(-1)).WillOnce(Return(nullptr));
    vm.setBytecode(bytecode3);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 10000);

    EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(&sprite1));
    vm.setBytecode(bytecode4);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 0);

    EXPECT_CALL(m_engineMock, targetAt(1)).WillOnce(Return(&sprite2));
    vm.setBytecode(bytecode5);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(std::round(vm.getInput(0, 1)->toDouble() * 10000) / 10000, 256.6178);

    EXPECT_CALL(m_engineMock, targetAt(-1)).WillOnce(Return(nullptr));
    vm.setBytecode(bytecode6);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 10000);

    EXPECT_CALL(m_engineMock, targetAt(2)).WillOnce(Return(nullptr));
    vm.setBytecode(bytecode7);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 10000);

    EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(168.087));
    EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(175.908));
    vm.setBytecode(bytecode8);
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(std::round(vm.getInput(0, 1)->toDouble() * 10000) / 10000, 261.0096);
}

TEST_F(SensingBlocksTest, KeyPressed)
{
    Compiler compiler(&m_engineMock);

    // key (space) pressed?
    auto block1 = std::make_shared<Block>("a", "sensing_keypressed");
    addDropdownInput(block1, "KEY_OPTION", SensingBlocks::KEY_OPTION, "space");

    // key (any) pressed?
    auto block2 = std::make_shared<Block>("b", "sensing_keypressed");
    addDropdownInput(block2, "KEY_OPTION", SensingBlocks::KEY_OPTION, "any");

    // key (null block) pressed?
    auto block3 = std::make_shared<Block>("c", "sensing_keypressed");
    addDropdownInput(block3, "KEY_OPTION", SensingBlocks::KEY_OPTION, "", createNullBlock("d"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::keyPressed)).WillOnce(Return(0));
    compiler.setBlock(block1);
    SensingBlocks::compileKeyPressed(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::keyPressed)).WillOnce(Return(0));
    compiler.setBlock(block2);
    SensingBlocks::compileKeyPressed(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::keyPressed)).WillOnce(Return(0));
    compiler.setBlock(block3);
    SensingBlocks::compileKeyPressed(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_NULL, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ "space", "any" }));
}

TEST_F(SensingBlocksTest, KeyPressedImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::keyPressed };
    static Value constValues[] = { "space", "any" };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, keyPressed("space")).WillOnce(Return(true));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toBool(), true);

    EXPECT_CALL(m_engineMock, keyPressed("space")).WillOnce(Return(false));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toBool(), false);

    EXPECT_CALL(m_engineMock, keyPressed("any")).WillOnce(Return(true));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toBool(), true);

    EXPECT_CALL(m_engineMock, keyPressed("any")).WillOnce(Return(false));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toBool(), false);
}

TEST_F(SensingBlocksTest, MouseDown)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sensing_mousedown");

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::mouseDown)).WillOnce(Return(0));
    compiler.init();

    compiler.setBlock(block);
    SensingBlocks::compileMouseDown(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(SensingBlocksTest, MouseDownImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::mouseDown };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, mousePressed()).WillOnce(Return(true));
    vm.setBytecode(bytecode);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toBool(), true);

    EXPECT_CALL(m_engineMock, mousePressed()).WillOnce(Return(false));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toBool(), false);
}

TEST_F(SensingBlocksTest, MouseX)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sensing_mousex");

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::mouseX)).WillOnce(Return(0));
    compiler.init();

    compiler.setBlock(block);
    SensingBlocks::compileMouseX(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(SensingBlocksTest, MouseXImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::mouseX };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(48.165));
    vm.setBytecode(bytecode);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 48.165);

    EXPECT_CALL(m_engineMock, mouseX()).WillOnce(Return(-239.09));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -239.09);
}

TEST_F(SensingBlocksTest, MouseY)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sensing_mousey");

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::mouseY)).WillOnce(Return(0));
    compiler.init();

    compiler.setBlock(block);
    SensingBlocks::compileMouseY(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(SensingBlocksTest, MouseYImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::mouseY };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(159.084));
    vm.setBytecode(bytecode);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 159.084);

    EXPECT_CALL(m_engineMock, mouseY()).WillOnce(Return(-95.564));
    vm.reset();
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -95.564);
}

TEST_F(SensingBlocksTest, SetDragMode)
{
    Compiler compiler(&m_engineMock);

    // set drag mode [draggable]
    auto block1 = std::make_shared<Block>("a", "sensing_setdragmode");
    addDropdownField(block1, "DRAG_MODE", SensingBlocks::DRAG_MODE, "draggable", SensingBlocks::Draggable);

    // set drag mode [not draggable]
    auto block2 = std::make_shared<Block>("b", "sensing_setdragmode");
    addDropdownField(block2, "DRAG_MODE", SensingBlocks::DRAG_MODE, "not draggable", SensingBlocks::NotDraggable);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::setDraggableMode)).WillOnce(Return(0));
    compiler.setBlock(block1);
    SensingBlocks::compileSetDragMode(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::setNotDraggableMode)).WillOnce(Return(1));
    compiler.setBlock(block2);
    SensingBlocks::compileSetDragMode(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
}

TEST_F(SensingBlocksTest, SetDragModeImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::setDraggableMode, &SensingBlocks::setNotDraggableMode };

    Sprite sprite;
    sprite.setDraggable(false);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(sprite.draggable());

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(sprite.draggable());
}

TEST_F(SensingBlocksTest, Timer)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sensing_timer");

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::timer)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    SensingBlocks::compileTimer(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
}

TEST_F(SensingBlocksTest, TimerImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::timer };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, timer()).WillOnce(Return(&m_timerMock));
    EXPECT_CALL(m_timerMock, value()).WillOnce(Return(2.375));

    vm.setBytecode(bytecode);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2.375);
}

TEST_F(SensingBlocksTest, ResetTimer)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sensing_resettimer");

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::resetTimer)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    SensingBlocks::compileResetTimer(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
}

TEST_F(SensingBlocksTest, ResetTimerImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::resetTimer };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);

    EXPECT_CALL(m_engineMock, timer()).WillOnce(Return(&m_timerMock));
    EXPECT_CALL(m_timerMock, reset()).Times(1);

    vm.setBytecode(bytecode);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}

TEST_F(SensingBlocksTest, Of)
{
    Compiler compiler(&m_engineMock);

    // [x position] of (Sprite2)
    auto block1 = std::make_shared<Block>("a", "sensing_of");
    addDropdownField(block1, "PROPERTY", SensingBlocks::PROPERTY, "x position", SensingBlocks::XPosition);
    addDropdownInput(block1, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [x position] of (null block)
    auto block2 = std::make_shared<Block>("b", "sensing_of");
    addDropdownField(block2, "PROPERTY", SensingBlocks::PROPERTY, "x position", SensingBlocks::XPosition);
    addDropdownInput(block2, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("c"));

    // [y position] of (Sprite2)
    auto block3 = std::make_shared<Block>("d", "sensing_of");
    addDropdownField(block3, "PROPERTY", SensingBlocks::PROPERTY, "y position", SensingBlocks::YPosition);
    addDropdownInput(block3, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [y position] of (null block)
    auto block4 = std::make_shared<Block>("e", "sensing_of");
    addDropdownField(block4, "PROPERTY", SensingBlocks::PROPERTY, "y position", SensingBlocks::YPosition);
    addDropdownInput(block4, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("f"));

    // [direction] of (Sprite2)
    auto block5 = std::make_shared<Block>("g", "sensing_of");
    addDropdownField(block5, "PROPERTY", SensingBlocks::PROPERTY, "direction", SensingBlocks::Direction);
    addDropdownInput(block5, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [direction] of (null block)
    auto block6 = std::make_shared<Block>("h", "sensing_of");
    addDropdownField(block6, "PROPERTY", SensingBlocks::PROPERTY, "direction", SensingBlocks::Direction);
    addDropdownInput(block6, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("i"));

    // [costume #] of (Sprite2)
    auto block7 = std::make_shared<Block>("j", "sensing_of");
    addDropdownField(block7, "PROPERTY", SensingBlocks::PROPERTY, "costume #", SensingBlocks::CostumeNumber);
    addDropdownInput(block7, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [costume #] of (null block)
    auto block8 = std::make_shared<Block>("k", "sensing_of");
    addDropdownField(block8, "PROPERTY", SensingBlocks::PROPERTY, "costume #", SensingBlocks::CostumeNumber);
    addDropdownInput(block8, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("l"));

    // [costume name] of (Sprite2)
    auto block9 = std::make_shared<Block>("m", "sensing_of");
    addDropdownField(block9, "PROPERTY", SensingBlocks::PROPERTY, "costume name", SensingBlocks::CostumeName);
    addDropdownInput(block9, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [costume name] of (null block)
    auto block10 = std::make_shared<Block>("n", "sensing_of");
    addDropdownField(block10, "PROPERTY", SensingBlocks::PROPERTY, "costume name", SensingBlocks::CostumeName);
    addDropdownInput(block10, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("o"));

    // [size] of (Sprite2)
    auto block11 = std::make_shared<Block>("p", "sensing_of");
    addDropdownField(block11, "PROPERTY", SensingBlocks::PROPERTY, "size", SensingBlocks::Size);
    addDropdownInput(block11, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [size] of (null block)
    auto block12 = std::make_shared<Block>("q", "sensing_of");
    addDropdownField(block12, "PROPERTY", SensingBlocks::PROPERTY, "size", SensingBlocks::Size);
    addDropdownInput(block12, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("r"));

    // [volume] of (Sprite2)
    auto block13 = std::make_shared<Block>("s", "sensing_of");
    addDropdownField(block13, "PROPERTY", SensingBlocks::PROPERTY, "volume", SensingBlocks::Volume);
    addDropdownInput(block13, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [volume] of (null block)
    auto block14 = std::make_shared<Block>("t", "sensing_of");
    addDropdownField(block14, "PROPERTY", SensingBlocks::PROPERTY, "volume", SensingBlocks::Volume);
    addDropdownInput(block14, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("u"));

    // [background #] of (Stage)
    auto block15 = std::make_shared<Block>("v", "sensing_of");
    addDropdownField(block15, "PROPERTY", SensingBlocks::PROPERTY, "background #", SensingBlocks::BackdropNumber);
    addDropdownInput(block15, "OBJECT", SensingBlocks::OBJECT, "_stage_");

    // [background #] of (null block)
    auto block16 = std::make_shared<Block>("w", "sensing_of");
    addDropdownField(block16, "PROPERTY", SensingBlocks::PROPERTY, "background #", SensingBlocks::BackdropNumber);
    addDropdownInput(block16, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("x"));

    // [backdrop #] of (Stage)
    auto block17 = std::make_shared<Block>("y", "sensing_of");
    addDropdownField(block17, "PROPERTY", SensingBlocks::PROPERTY, "backdrop #", SensingBlocks::BackdropNumber);
    addDropdownInput(block17, "OBJECT", SensingBlocks::OBJECT, "_stage_");

    // [backdrop #] of (null block)
    auto block18 = std::make_shared<Block>("z", "sensing_of");
    addDropdownField(block18, "PROPERTY", SensingBlocks::PROPERTY, "backdrop #", SensingBlocks::BackdropNumber);
    addDropdownInput(block18, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("aa"));

    // [backdrop name] of (Stage)
    auto block19 = std::make_shared<Block>("ab", "sensing_of");
    addDropdownField(block19, "PROPERTY", SensingBlocks::PROPERTY, "backdrop name", SensingBlocks::BackdropName);
    addDropdownInput(block19, "OBJECT", SensingBlocks::OBJECT, "_stage_");

    // [backdrop name] of (null block)
    auto block20 = std::make_shared<Block>("ac", "sensing_of");
    addDropdownField(block20, "PROPERTY", SensingBlocks::PROPERTY, "backdrop name", SensingBlocks::BackdropName);
    addDropdownInput(block20, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("ad"));

    // [some variable] of (Stage)
    auto block21 = std::make_shared<Block>("ae", "sensing_of");
    addDropdownField(block21, "PROPERTY", SensingBlocks::PROPERTY, "some variable", static_cast<SensingBlocks::FieldValues>(-1));
    addDropdownInput(block21, "OBJECT", SensingBlocks::OBJECT, "_stage_");

    // [some invalid variable] of (Stage)
    auto block22 = std::make_shared<Block>("af", "sensing_of");
    addDropdownField(block22, "PROPERTY", SensingBlocks::PROPERTY, "some invalid variable", static_cast<SensingBlocks::FieldValues>(-1));
    addDropdownInput(block22, "OBJECT", SensingBlocks::OBJECT, "_stage_");

    // [some variable] of (Sprite2)
    auto block23 = std::make_shared<Block>("ag", "sensing_of");
    addDropdownField(block23, "PROPERTY", SensingBlocks::PROPERTY, "some variable", static_cast<SensingBlocks::FieldValues>(-1));
    addDropdownInput(block23, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [some invalid variable] of (Sprite2)
    auto block24 = std::make_shared<Block>("ah", "sensing_of");
    addDropdownField(block24, "PROPERTY", SensingBlocks::PROPERTY, "some invalid variable", static_cast<SensingBlocks::FieldValues>(-1));
    addDropdownInput(block24, "OBJECT", SensingBlocks::OBJECT, "Sprite2");

    // [some variable] of (null block)
    auto block25 = std::make_shared<Block>("ai", "sensing_of");
    addDropdownField(block25, "PROPERTY", SensingBlocks::PROPERTY, "some variable", static_cast<SensingBlocks::FieldValues>(-1));
    addDropdownInput(block25, "OBJECT", SensingBlocks::OBJECT, "", createNullBlock("aj"));

    compiler.init();

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).Times(9).WillRepeatedly(Return(6));
    EXPECT_CALL(m_engineMock, findTarget("_stage_")).Times(5).WillRepeatedly(Return(0));

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::xPositionOfSpriteByIndex)).WillOnce(Return(0));
    compiler.setBlock(block1);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::xPositionOfSprite)).WillOnce(Return(1));
    compiler.setBlock(block2);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::yPositionOfSpriteByIndex)).WillOnce(Return(2));
    compiler.setBlock(block3);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::yPositionOfSprite)).WillOnce(Return(3));
    compiler.setBlock(block4);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::directionOfSpriteByIndex)).WillOnce(Return(4));
    compiler.setBlock(block5);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::directionOfSprite)).WillOnce(Return(5));
    compiler.setBlock(block6);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::costumeNumberOfSpriteByIndex)).WillOnce(Return(6));
    compiler.setBlock(block7);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::costumeNumberOfSprite)).WillOnce(Return(7));
    compiler.setBlock(block8);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::costumeNameOfSpriteByIndex)).WillOnce(Return(8));
    compiler.setBlock(block9);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::costumeNameOfSprite)).WillOnce(Return(9));
    compiler.setBlock(block10);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::sizeOfSpriteByIndex)).WillOnce(Return(10));
    compiler.setBlock(block11);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::sizeOfSprite)).WillOnce(Return(11));
    compiler.setBlock(block12);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::volumeOfTargetByIndex)).WillOnce(Return(12));
    compiler.setBlock(block13);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::volumeOfTarget)).WillOnce(Return(13));
    compiler.setBlock(block14);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::backdropNumberOfStageByIndex)).WillOnce(Return(14));
    compiler.setBlock(block15);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::backdropNumberOfStage)).WillOnce(Return(15));
    compiler.setBlock(block16);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::backdropNumberOfStageByIndex)).WillOnce(Return(14));
    compiler.setBlock(block17);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::backdropNumberOfStage)).WillOnce(Return(15));
    compiler.setBlock(block18);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::backdropNameOfStageByIndex)).WillOnce(Return(16));
    compiler.setBlock(block19);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::backdropNameOfStage)).WillOnce(Return(17));
    compiler.setBlock(block20);
    SensingBlocks::compileOf(&compiler);

    Stage stage;
    auto v1 = std::make_shared<Variable>("var1", "test");
    auto v2 = std::make_shared<Variable>("var2", "some variable");
    stage.addVariable(v1);
    stage.addVariable(v2);
    EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(&stage));
    compiler.setBlock(block21);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(&stage));
    compiler.setBlock(block22);
    SensingBlocks::compileOf(&compiler);

    Sprite sprite;
    auto v3 = std::make_shared<Variable>("var3", "some variable");
    sprite.addVariable(v3);
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    compiler.setBlock(block23);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    compiler.setBlock(block24);
    SensingBlocks::compileOf(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::variableOfTarget)).WillOnce(Return(18));
    compiler.setBlock(block25);
    SensingBlocks::compileOf(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START,
              vm::OP_CONST,
              0,
              vm::OP_EXEC,
              0,
              vm::OP_NULL,
              vm::OP_EXEC,
              1,
              vm::OP_CONST,
              1,
              vm::OP_EXEC,
              2,
              vm::OP_NULL,
              vm::OP_EXEC,
              3,
              vm::OP_CONST,
              2,
              vm::OP_EXEC,
              4,
              vm::OP_NULL,
              vm::OP_EXEC,
              5,
              vm::OP_CONST,
              3,
              vm::OP_EXEC,
              6,
              vm::OP_NULL,
              vm::OP_EXEC,
              7,
              vm::OP_CONST,
              4,
              vm::OP_EXEC,
              8,
              vm::OP_NULL,
              vm::OP_EXEC,
              9,
              vm::OP_CONST,
              5,
              vm::OP_EXEC,
              10,
              vm::OP_NULL,
              vm::OP_EXEC,
              11,
              vm::OP_CONST,
              6,
              vm::OP_EXEC,
              12,
              vm::OP_NULL,
              vm::OP_EXEC,
              13,
              vm::OP_CONST,
              7,
              vm::OP_EXEC,
              14,
              vm::OP_NULL,
              vm::OP_EXEC,
              15,
              vm::OP_CONST,
              8,
              vm::OP_EXEC,
              14,
              vm::OP_NULL,
              vm::OP_EXEC,
              15,
              vm::OP_CONST,
              9,
              vm::OP_EXEC,
              16,
              vm::OP_NULL,
              vm::OP_EXEC,
              17,
              vm::OP_READ_VAR,
              0,
              vm::OP_NULL,
              vm::OP_READ_VAR,
              1,
              vm::OP_NULL,
              vm::OP_CONST,
              10,
              vm::OP_NULL,
              vm::OP_EXEC,
              18,
              vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, "some variable" }));
    ASSERT_EQ(compiler.variables(), std::vector<Variable *>({ v2.get(), v3.get() }));
}

TEST_F(SensingBlocksTest, XPositionOfSprite)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::xPositionOfSprite, &SensingBlocks::xPositionOfSpriteByIndex };
    static Value constValues[] = { "Sprite2", 6 };

    Sprite sprite;
    sprite.setX(-168.088);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -168.088);

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -168.088);
}

TEST_F(SensingBlocksTest, YPositionOfSprite)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::yPositionOfSprite, &SensingBlocks::yPositionOfSpriteByIndex };
    static Value constValues[] = { "Sprite2", 6 };

    Sprite sprite;
    sprite.setY(-16.548);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -16.548);

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -16.548);
}

TEST_F(SensingBlocksTest, DirectionOfSprite)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::directionOfSprite, &SensingBlocks::directionOfSpriteByIndex };
    static Value constValues[] = { "Sprite2", 6 };

    Sprite sprite;
    sprite.setDirection(-109.087);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -109.087);

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), -109.087);
}

TEST_F(SensingBlocksTest, CostumeNumberOfSprite)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::costumeNumberOfSprite, &SensingBlocks::costumeNumberOfSpriteByIndex };
    static Value constValues[] = { "Sprite2", 6 };

    Sprite sprite;
    sprite.addCostume(std::make_shared<Costume>("costume1", "a", "png"));
    sprite.addCostume(std::make_shared<Costume>("costume2", "b", "svg"));
    sprite.setCostumeIndex(1);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);
}

TEST_F(SensingBlocksTest, CostumeNameOfSprite)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::costumeNameOfSprite, &SensingBlocks::costumeNameOfSpriteByIndex };
    static Value constValues[] = { "Sprite2", 6 };

    Sprite sprite;
    sprite.addCostume(std::make_shared<Costume>("costume1", "a", "png"));
    sprite.addCostume(std::make_shared<Costume>("costume2", "b", "svg"));
    sprite.setCostumeIndex(1);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "costume2");

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "costume2");
}

TEST_F(SensingBlocksTest, SizeOfSprite)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::sizeOfSprite, &SensingBlocks::sizeOfSpriteByIndex };
    static Value constValues[] = { "Sprite2", 6 };

    Sprite sprite;
    sprite.setSize(48.642);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 48.642);

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 48.642);
}

TEST_F(SensingBlocksTest, VolumeOfTarget)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::volumeOfTarget, &SensingBlocks::volumeOfTargetByIndex };
    static Value constValues[] = { "Sprite2", "_stage_", 6, 0 };

    Sprite sprite;
    sprite.setVolume(85);

    Stage stage;
    stage.setVolume(48);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), sprite.volume());

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), stage.volume());

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), sprite.volume());

    EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), stage.volume());
}

TEST_F(SensingBlocksTest, VariableOfTarget)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::variableOfTarget };
    static Value constValues[] = { "variable", "invalid variable", "Sprite2", "_stage_", "test" };

    Sprite sprite;
    auto v1 = std::make_shared<Variable>("var1", "variable", 64.13);
    sprite.addVariable(v1);

    Stage stage;
    auto v2 = std::make_shared<Variable>("var2", "test", 98);
    auto v3 = std::make_shared<Variable>("var3", "variable", -0.85);
    stage.addVariable(v2);
    stage.addVariable(v3);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), v1->value().toDouble());

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&sprite));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 0);

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), v3->value().toDouble());

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(0));
    EXPECT_CALL(m_engineMock, targetAt(0)).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 0);

    EXPECT_CALL(m_engineMock, findTarget("test")).WillOnce(Return(-1));
    EXPECT_CALL(m_engineMock, targetAt(-1)).WillOnce(Return(nullptr));
    vm.reset();
    vm.setBytecode(bytecode5);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 0);
}

TEST_F(SensingBlocksTest, BackdropNumberOfStage)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::backdropNumberOfStage, &SensingBlocks::backdropNumberOfStageByIndex };
    static Value constValues[] = { "_stage_", 6 };

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "a", "png"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b", "svg"));
    stage.setCostumeIndex(1);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&stage));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 2);
}

TEST_F(SensingBlocksTest, BackdropNameOfStage)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::backdropNameOfStage, &SensingBlocks::backdropNameOfStageByIndex };
    static Value constValues[] = { "_stage_", 6 };

    Stage stage;
    stage.addCostume(std::make_shared<Costume>("backdrop1", "a", "png"));
    stage.addCostume(std::make_shared<Costume>("backdrop2", "b", "svg"));
    stage.setCostumeIndex(1);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillOnce(Return(6));
    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&stage));
    vm.setBytecode(bytecode1);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "backdrop2");

    EXPECT_CALL(m_engineMock, targetAt(6)).WillOnce(Return(&stage));
    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toString(), "backdrop2");
}

TEST_F(SensingBlocksTest, Current)
{
    Compiler compiler(&m_engineMock);

    // current [year]
    auto block1 = createSensingCurrentBlock("a", "YEAR", SensingBlocks::YEAR);

    // current [month]
    auto block2 = createSensingCurrentBlock("b", "MONTH", SensingBlocks::MONTH);

    // current [date]
    auto block3 = createSensingCurrentBlock("c", "DATE", SensingBlocks::DATE);

    // current [day of week]
    auto block4 = createSensingCurrentBlock("d", "DAYOFWEEK", SensingBlocks::DAYOFWEEK);

    // current [hour]
    auto block5 = createSensingCurrentBlock("e", "HOUR", SensingBlocks::HOUR);

    // current [minute]
    auto block6 = createSensingCurrentBlock("f", "MINUTE", SensingBlocks::MINUTE);

    // current [second]
    auto block7 = createSensingCurrentBlock("g", "SECOND", SensingBlocks::SECOND);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::currentYear)).WillOnce(Return(0));
    compiler.setBlock(block1);
    SensingBlocks::compileCurrent(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::currentMonth)).WillOnce(Return(1));
    compiler.setBlock(block2);
    SensingBlocks::compileCurrent(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::currentDate)).WillOnce(Return(2));
    compiler.setBlock(block3);
    SensingBlocks::compileCurrent(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::currentDayOfWeek)).WillOnce(Return(3));
    compiler.setBlock(block4);
    SensingBlocks::compileCurrent(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::currentHour)).WillOnce(Return(4));
    compiler.setBlock(block5);
    SensingBlocks::compileCurrent(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::currentMinute)).WillOnce(Return(5));
    compiler.setBlock(block6);
    SensingBlocks::compileCurrent(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::currentSecond)).WillOnce(Return(6));
    compiler.setBlock(block7);
    SensingBlocks::compileCurrent(&compiler);
    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_EXEC, 2, vm::OP_EXEC, 3, vm::OP_EXEC, 4, vm::OP_EXEC, 5, vm::OP_EXEC, 6, vm::OP_HALT }));
}

TEST_F(SensingBlocksTest, CurrentYear)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::currentYear };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), ltm->tm_year + 1900);
}

TEST_F(SensingBlocksTest, CurrentMonth)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::currentMonth };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), ltm->tm_mon + 1);
}

TEST_F(SensingBlocksTest, CurrentDate)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::currentDate };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), ltm->tm_mday);
}

TEST_F(SensingBlocksTest, CurrentDayOfWeek)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::currentDayOfWeek };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), ltm->tm_wday + 1);
}

TEST_F(SensingBlocksTest, CurrentHour)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::currentHour };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), ltm->tm_hour);
}

TEST_F(SensingBlocksTest, CurrentMinute)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::currentMinute };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), ltm->tm_min);
}

TEST_F(SensingBlocksTest, CurrentSecond)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::currentSecond };

    VirtualMachine vm;
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);
    vm.run();

    ASSERT_EQ(vm.registerCount(), 1);

    time_t now = time(0);
    tm *ltm = localtime(&now);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), ltm->tm_sec);
}

TEST_F(SensingBlocksTest, DaysSince2000)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "sensing_dayssince2000");

    EXPECT_CALL(m_engineMock, functionIndex(&SensingBlocks::daysSince2000)).WillOnce(Return(0));

    compiler.init();
    compiler.setBlock(block);
    SensingBlocks::compileDaysSince2000(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT }));
}

TEST_F(SensingBlocksTest, DaysSince2000Impl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &SensingBlocks::daysSince2000 };

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setFunctions(functions);
    vm.setBytecode(bytecode);

    std::chrono::system_clock::time_point time(std::chrono::milliseconds(1011243120562)); // Jan 17 2002 04:52:00
    EXPECT_CALL(m_clockMock, currentSystemTime()).WillOnce(Return(time));

    vm.run(); // Test with the default clock - shouldn't crash (see #210)

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(std::round(vm.getInput(0, 1)->toDouble() * 100) / 100, std::round((ms / 86400000.0 - 10957) * 100) / 100);

    SensingBlocks::clock = &m_clockMock;
    vm.reset();
    vm.run();
    SensingBlocks::clock = nullptr;

    ASSERT_EQ(vm.registerCount(), 1);
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 747.20278428240817);
}
