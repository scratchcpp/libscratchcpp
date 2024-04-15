// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "../iaudioengine.h"

struct ma_engine;

namespace libscratchcpp
{

// This is a singleton which initializes and uninitializes the miniaudio engine
class AudioEngine : public IAudioEngine
{
    public:
        friend class IAudioEngine;

        static ma_engine *engine();
        static bool initialized();

        float volume() const override;
        void setVolume(float volume) override;

    private:
        AudioEngine();
        ~AudioEngine();

        void init();

        static AudioEngine instance;

        ma_engine *m_engine = nullptr;
        bool m_initialized = false;
        float m_volume = 1.0f;
};

} // namespace libscratchcpp
