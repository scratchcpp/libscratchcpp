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
        bool loadCopy(IAudioPlayer *player) override;

        float volume() const override;
        void setVolume(float volume) override;

        float pitch() const override;
        void setPitch(float pitch) override;

        float pan() const override;
        void setPan(float pan) override;

        bool isLoaded() const override;

        void start() override;
        void stop() override;

        bool isPlaying() const override;

    private:
        float m_volume = 1;
        float m_pitch = 1;
        float m_pan = 0;
};

} // namespace libscratchcpp
