// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <miniaudio.h>

#include "audioengine.h"

using namespace libscratchcpp;

static AudioEngine INSTANCE;

IAudioEngine *IAudioEngine::instance()
{
    return &INSTANCE;
}

ma_engine *AudioEngine::engine()
{
    if (!INSTANCE.m_initialized)
        INSTANCE.init();

    return INSTANCE.m_initialized ? INSTANCE.m_engine : nullptr;
}

bool AudioEngine::initialized()
{
    if (!INSTANCE.m_initialized)
        INSTANCE.init();

    return INSTANCE.m_initialized;
}

float AudioEngine::volume() const
{
    return m_volume;
}

void AudioEngine::setVolume(float volume)
{
    m_volume = volume;

    if (m_initialized)
        ma_engine_set_volume(m_engine, volume);
}

AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
    if (m_initialized && m_engine) {
        ma_engine_uninit(m_engine);
        delete m_engine;
    }
}

void AudioEngine::init()
{
    ma_result result;
    m_engine = new ma_engine;
    result = ma_engine_init(NULL, m_engine);

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
        return;
    }

    m_initialized = true;
}
