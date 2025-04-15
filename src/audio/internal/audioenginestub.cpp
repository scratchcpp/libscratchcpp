// SPDX-License-Identifier: Apache-2.0

#include "audioenginestub.h"

using namespace libscratchcpp;

static AudioEngineStub INSTANCE;

IAudioEngine *IAudioEngine::instance()
{
    return &INSTANCE;
}

AudioEngineStub::AudioEngineStub()
{
}

float AudioEngineStub::volume() const
{
    return m_volume;
}

void AudioEngineStub::setVolume(float volume)
{
    m_volume = volume;
}
