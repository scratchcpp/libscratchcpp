// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "audioengine.h"

using namespace libscratchcpp;

AudioEngine AudioEngine::instance;

ma_engine *AudioEngine::engine()
{
    if (!instance.m_initialized)
        instance.init();

    return instance.m_initialized ? &instance.m_engine : nullptr;
}

bool AudioEngine::initialized()
{
    if (!instance.m_initialized)
        instance.init();

    return instance.m_initialized;
}

AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
    if (m_initialized)
        ma_engine_uninit(&m_engine);
}

void AudioEngine::init()
{
    ma_result result;
    result = ma_engine_init(NULL, &m_engine);

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
        return;
    }

    m_initialized = true;
}
