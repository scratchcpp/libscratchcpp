#pragma once

#include <engine/internal/iclock.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ClockMock : public IClock
{
    public:
        MOCK_METHOD(std::chrono::steady_clock::time_point, currentTime, (), (const override));
};
