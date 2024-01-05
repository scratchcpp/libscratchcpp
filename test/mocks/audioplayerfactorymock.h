#pragma once

#include <audio/iaudioplayerfactory.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class AudioPlayerFactoryMock : public IAudioPlayerFactory
{
    public:
        MOCK_METHOD(std::shared_ptr<IAudioPlayer>, create, (), (const, override));
};
