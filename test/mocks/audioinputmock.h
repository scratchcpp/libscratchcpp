#pragma once

#include <audio/iaudioinput.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class AudioInputMock : public IAudioInput
{
    public:
        MOCK_METHOD(std::shared_ptr<IAudioLoudness>, audioLoudness, (), (const, override));
};
