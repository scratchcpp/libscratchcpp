#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/field.h>
#include <enginemock.h>
#include <timermock.h>
#include <clockmock.h>

#include "../common.h"
#include "blocks/sensingblocks.h"
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
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_timer", &SensingBlocks::compileTimer)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_resettimer", &SensingBlocks::compileResetTimer)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_current", &SensingBlocks::compileCurrent)).Times(1);
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_dayssince2000", &SensingBlocks::compileDaysSince2000)).Times(1);

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
