#include <engine/internal/stacktimer.h>

#include <clockmock.h>

using namespace libscratchcpp;

using ::testing::Return;

TEST(StackTimerTest, StartStopElapsed)
{
    ClockMock clock;
    StackTimer timer(&clock);

    ASSERT_TRUE(timer.stopped());

    EXPECT_CALL(clock, currentSteadyTime).Times(0);
    ASSERT_FALSE(timer.elapsed());

    std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(73));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    timer.start(0.5);

    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    ASSERT_FALSE(timer.elapsed());
    ASSERT_FALSE(timer.stopped());

    std::chrono::steady_clock::time_point time3(std::chrono::milliseconds(520));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time3));
    ASSERT_FALSE(timer.elapsed());
    ASSERT_FALSE(timer.stopped());

    std::chrono::steady_clock::time_point time4(std::chrono::milliseconds(573));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time4));
    ASSERT_TRUE(timer.elapsed());
    ASSERT_FALSE(timer.stopped());

    std::chrono::steady_clock::time_point time5(std::chrono::milliseconds(580));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time5));
    ASSERT_TRUE(timer.elapsed());
    ASSERT_FALSE(timer.stopped());

    timer.stop();
    EXPECT_CALL(clock, currentSteadyTime).Times(0);
    ASSERT_FALSE(timer.elapsed());
    ASSERT_TRUE(timer.stopped());
}
