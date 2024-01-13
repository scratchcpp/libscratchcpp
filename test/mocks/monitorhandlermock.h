#pragma once

#include <scratchcpp/imonitorhandler.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class MonitorHandlerMock : public IMonitorHandler
{
    public:
        MOCK_METHOD(void, init, (Monitor *), (override));

        MOCK_METHOD(void, onValueChanged, (const VirtualMachine *), (override));
        MOCK_METHOD(void, onVisibleChanged, (bool), (override));
};
