#pragma once

#include <audio/iaudioplayer.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class AudioPlayerMock : public IAudioPlayer
{
    public:
        MOCK_METHOD(bool, load, (unsigned int, const void *, unsigned long), (override));

        MOCK_METHOD(void, start, (), (override));
        MOCK_METHOD(void, stop, (), (override));

        MOCK_METHOD(bool, isPlaying, (), (override, const));
};
