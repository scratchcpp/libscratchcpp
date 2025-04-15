#pragma once

#include <scratchcpp/sound.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class SoundMock : public Sound
{
    public:
        SoundMock() :
            Sound("", "", "")
        {
        }

        MOCK_METHOD(void, setVolume, (double), (override));
        MOCK_METHOD(void, setEffect, (Sound::Effect, double), (override));

        MOCK_METHOD(void, start, (Thread *), (override));
        MOCK_METHOD(void, stop, (), (override));
        MOCK_METHOD(bool, isPlaying, (), (const, override));
};
