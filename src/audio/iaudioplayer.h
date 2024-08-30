// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class IAudioPlayer
{
    public:
        virtual ~IAudioPlayer() { }

        virtual bool load(unsigned int size, const void *data, unsigned long sampleRate) = 0;
        virtual bool loadCopy(IAudioPlayer *player) = 0;

        virtual float volume() const = 0;
        virtual void setVolume(float volume) = 0;

        virtual float pitch() const = 0;
        virtual void setPitch(float pitch) = 0;

        virtual bool isLoaded() const = 0;

        virtual void start() = 0;
        virtual void stop() = 0;

        virtual bool isPlaying() const = 0;
};

} // namespace libscratchcpp
