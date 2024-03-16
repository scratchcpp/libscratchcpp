#pragma once

#include <audio/iaudioloudness.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class AudioLoudnessMock : public IAudioLoudness
{
    public:
        MOCK_METHOD(int, getLoudness, (), (const, override));
};
