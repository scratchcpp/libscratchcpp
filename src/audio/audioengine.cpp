// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "audioengine.h"

using namespace libscratchcpp;

AudioEngine AudioEngine::instance;

ma_engine *AudioEngine::engine()
{
    return &instance.m_engine;
}

bool AudioEngine::initialized()
{
    return instance.m_initialized;
}

AudioEngine::AudioEngine()
{
    ma_result result;
    result = ma_engine_init(NULL, &m_engine);

    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
        return;
    }

    m_initialized = true;
}

AudioEngine::~AudioEngine()
{
    if (m_initialized)
        ma_engine_uninit(&m_engine);
}
