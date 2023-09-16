#pragma once

#include <scratchcpp/itimer.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class TimerMock : public ITimer
{
    public:
        MOCK_METHOD(double, value, (), (const, override));

        MOCK_METHOD(void, reset, (), (override));
};
