#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
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
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "sensing_dayssince2000", &SensingBlocks::compileDaysSince2000)).Times(1);

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

    ASSERT_EQ(vm.registerCount(), 2); // TODO: Change this to 1 after task #215 is completed
    ASSERT_EQ(vm.getInput(0, 1)->toDouble(), 747.20278428240817);
}
