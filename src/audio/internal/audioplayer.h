// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioplayer.h"

struct ma_decoder;
struct ma_sound;

namespace libscratchcpp
{

class AudioPlayer : public IAudioPlayer
{
    public:
        AudioPlayer();
        ~AudioPlayer();

        bool load(unsigned int size, const void *data, unsigned long sampleRate) override;
        bool loadCopy(IAudioPlayer *player) override;

        float volume() const override;
        void setVolume(float volume) override;

        bool isLoaded() const override;

        void start() override;
        void stop() override;

        bool isPlaying() const override;

    private:
        ma_decoder *m_decoder = nullptr;
        ma_sound *m_sound;
        bool m_loaded = false;
        bool m_copy = false;
        bool m_started = false;
        float m_volume = 1;
};

} // namespace libscratchcpp
