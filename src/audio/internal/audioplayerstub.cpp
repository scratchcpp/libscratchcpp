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

bool AudioPlayerStub::loadCopy(IAudioPlayer *player)
{
    return true;
}

float AudioPlayerStub::volume() const
{
    return m_volume;
}

void AudioPlayerStub::setVolume(float volume)
{
    m_volume = volume;
}

float AudioPlayerStub::pitch() const
{
    return m_pitch;
}

void AudioPlayerStub::setPitch(float pitch)
{
    m_pitch = pitch;
}

bool AudioPlayerStub::isLoaded() const
{
    return true;
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
