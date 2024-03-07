// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <miniaudio.h>

namespace libscratchcpp
{

// This is a singleton which initializes and uninitializes the miniaudio engine
class AudioEngine
{
    public:
        static ma_engine *engine();
        static bool initialized();

    private:
        AudioEngine();
        ~AudioEngine();

        void init();

        static AudioEngine instance;
        ma_engine m_engine;
        bool m_initialized = false;
};

} // namespace libscratchcpp
