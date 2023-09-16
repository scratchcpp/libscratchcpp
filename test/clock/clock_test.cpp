#include <engine/internal/clock.h>
#include <cmath>

#include "../common.h"

using namespace libscratchcpp;

TEST(ClockTest, CurrentTime)
{
    auto clock = Clock::instance();
    ASSERT_TRUE(clock);

    auto currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    double currentTimeMs = std::round(currentTime.time_since_epoch().count() / 10) * 10;

    auto reportedTime = std::chrono::time_point_cast<std::chrono::milliseconds>(clock->currentTime());
    double reportedTimeMs = std::round(reportedTime.time_since_epoch().count() / 10) * 10;

    ASSERT_EQ(reportedTimeMs, currentTimeMs);
}
