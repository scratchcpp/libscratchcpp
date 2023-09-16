#include <engine/internal/timer.h>
#include <clockmock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

TEST(TimerTest, ValueAndReset)
{
    ClockMock clock;

    std::chrono::steady_clock::time_point startTime(std::chrono::milliseconds(50));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(startTime));
    Timer timer(&clock);

    std::chrono::steady_clock::time_point time1(std::chrono::milliseconds(73));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time1));
    ASSERT_EQ(timer.value(), 0.023);

    std::chrono::steady_clock::time_point time2(std::chrono::milliseconds(15432));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time2));
    ASSERT_EQ(timer.value(), 15.382);

    std::chrono::steady_clock::time_point time3(std::chrono::milliseconds(16025));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time3));
    timer.reset();

    std::chrono::steady_clock::time_point time4(std::chrono::milliseconds(24632));
    EXPECT_CALL(clock, currentSteadyTime()).WillOnce(Return(time4));
    ASSERT_EQ(timer.value(), 8.607);
}
