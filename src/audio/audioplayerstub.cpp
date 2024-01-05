// SPDX-License-Identifier: Apache-2.0

#include "audioplayerstub.h"

using namespace libscratchcpp;

AudioPlayerStub::AudioPlayerStub()
{
}

bool AudioPlayerStub::load(unsigned int size, const void *data, unsigned long sampleRate)
{
    return true;
}

void AudioPlayerStub::setVolume(float volume)
{
}

void AudioPlayerStub::start()
{
}

void AudioPlayerStub::stop()
{
}

bool AudioPlayerStub::isPlaying() const
{
    return false;
}
