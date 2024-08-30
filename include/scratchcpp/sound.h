// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "spimpl.h"

#include "asset.h"

namespace libscratchcpp
{

class Target;
class SoundPrivate;

/*! \brief The Sound class represents a Scratch sound. */
class LIBSCRATCHCPP_EXPORT Sound : public Asset
{
    public:
        enum class Effect
        {
            Pitch
        };

        Sound(const std::string &name, const std::string &id, const std::string &format);
        Sound(const Sound &) = delete;
        virtual ~Sound() { }

        int rate() const;
        void setRate(int newRate);

        int sampleCount() const;
        void setSampleCount(int newSampleCount);

        virtual void setVolume(double volume);
        virtual void setEffect(Effect effect, double value);

        virtual void start();
        virtual void stop();

        virtual bool isPlaying();

        Target *target() const;
        void setTarget(Target *target);

        std::shared_ptr<Sound> clone() const;

    protected:
        void processData(unsigned int size, void *data) override;

    private:
        void stopCloneSounds();

        spimpl::unique_impl_ptr<SoundPrivate> impl;
};

} // namespace libscratchcpp
