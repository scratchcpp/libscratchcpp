#pragma once

#include <engine/internal/iclock.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ClockMock : public IClock
{
    public:
        MOCK_METHOD(std::chrono::steady_clock::time_point, currentSteadyTime, (), (const, override));
        MOCK_METHOD(std::chrono::system_clock::time_point, currentSystemTime, (), (const, override));
};
