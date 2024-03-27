// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioplayer.h"

namespace libscratchcpp
{

class AudioPlayerStub : public IAudioPlayer
{
    public:
        AudioPlayerStub();

        bool load(unsigned int size, const void *data, unsigned long sampleRate) override;
        void setVolume(float volume) override;

        void start() override;
        void stop() override;

        bool isPlaying() const override;
};

} // namespace libscratchcpp
