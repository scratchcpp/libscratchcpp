#pragma once

#include <audio/iaudioengine.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class AudioEngineMock : public IAudioEngine
{
    public:
        MOCK_METHOD(float, volume, (), (const, override));
        MOCK_METHOD(void, setVolume, (float), (override));
};
