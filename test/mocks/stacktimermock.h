#pragma once

#include <scratchcpp/istacktimer.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class StackTimerMock : public IStackTimer
{
    public:
        MOCK_METHOD(void, start, (double), (override));
        MOCK_METHOD(void, stop, (), (override));

        MOCK_METHOD(bool, stopped, (), (const, override));
        MOCK_METHOD(bool, elapsed, (), (const, override));
        MOCK_METHOD(double, elapsedTime, (), (const, override));
};
