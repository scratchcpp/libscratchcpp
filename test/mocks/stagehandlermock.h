#pragma once

#include <scratchcpp/istagehandler.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class StageHandlerMock : public IStageHandler
{
    public:
        MOCK_METHOD(void, init, (Stage *), (override));

        MOCK_METHOD(void, onCostumeChanged, (Costume *), (override));

        MOCK_METHOD(void, onTempoChanged, (int), (override));
        MOCK_METHOD(void, onVideoStateChanged, (Stage::VideoState), (override));
        MOCK_METHOD(void, onVideoTransparencyChanged, (int), (override));
};
