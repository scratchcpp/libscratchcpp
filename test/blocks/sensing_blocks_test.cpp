#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
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
            block->updateFieldMap();

            return block;
        }

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, SensingBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();
        }

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, SensingBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);
            block->updateInputMap();

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
            block->updateFieldMap();
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
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_timer", &SensingBlocks::compileTimer));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_resettimer", &SensingBlocks::compileResetTimer));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_current", &SensingBlocks::compileCurrent));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_dayssince2000", &SensingBlocks::compileDaysSince2000));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "DISTANCETOMENU", SensingBlocks::DISTANCETOMENU));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "KEY_OPTION", SensingBlocks::KEY_OPTION));

    // Fields
    EXPECT_CALL(m_engineMock, addField(m_section.get(), "CURRENTMENU", SensingBlocks::CURRENTMENU));

    // Field values
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "YEAR", SensingBlocks::YEAR));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "MONTH", SensingBlocks::MONTH));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "DATE", SensingBlocks::DATE));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "DAYOFWEEK", SensingBlocks::DAYOFWEEK));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "HOUR", SensingBlocks::HOUR));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "MINUTE", SensingBlocks::MINUTE));
    EXPECT_CALL(m_engineMock, addFieldValue(m_section.get(), "SECOND", SensingBlocks::SECOND));

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

    // distance to (join "" "")
    auto joinBlock = std::make_shared<Block>("d", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block3 = std::make_shared<Block>("c", "sensing_distanceto");
    addDropdownInput(block3, "DISTANCETOMENU", SensingBlocks::DISTANCETOMENU, "", joinBlock);

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

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_EXEC, 1, vm::OP_NULL, vm::OP_NULL, vm::OP_STR_CONCAT, vm::OP_EXEC, 2, vm::OP_HALT }));
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

    // key (join "" "") pressed?
    auto joinBlock = std::make_shared<Block>("d", "operator_join");
    joinBlock->setCompileFunction(&OperatorBlocks::compileJoin);
    auto block3 = std::make_shared<Block>("c", "sensing_keypressed");
    addDropdownInput(block3, "KEY_OPTION", SensingBlocks::KEY_OPTION, "", joinBlock);

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

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_NULL, vm::OP_NULL, vm::OP_STR_CONCAT, vm::OP_EXEC, 0, vm::OP_HALT }));
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
