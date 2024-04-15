// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioengine.h"

namespace libscratchcpp
{

class AudioEngineStub : public IAudioEngine
{
    public:
        friend class IAudioEngine;
        AudioEngineStub();

        float volume() const override;
        void setVolume(float volume) override;

    private:
        static AudioEngineStub instance;
        float m_volume = 1.0f;
};

} // namespace libscratchcpp
