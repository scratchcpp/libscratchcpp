// SPDX-License-Identifier: Apache-2.0

#include "audioenginestub.h"

using namespace libscratchcpp;

AudioEngineStub AudioEngineStub::instance;

IAudioEngine *IAudioEngine::instance()
{
    return &AudioEngineStub::instance;
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
