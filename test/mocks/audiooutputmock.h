#pragma once

#include <audio/iaudiooutput.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class AudioOutputMock : public IAudioOutput
{
    public:
        MOCK_METHOD(std::shared_ptr<IAudioPlayer>, createAudioPlayer, (), (const, override));
};
