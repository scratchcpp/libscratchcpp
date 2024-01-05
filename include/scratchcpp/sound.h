// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "spimpl.h"

#include "asset.h"

namespace libscratchcpp
{

class SoundPrivate;

/*! \brief The Sound class represents a Scratch sound. */
class LIBSCRATCHCPP_EXPORT Sound : public Asset
{
    public:
        Sound(const std::string &name, const std::string &id, const std::string &format);
        Sound(const Sound &) = delete;
        virtual ~Sound() { }

        int rate() const;
        void setRate(int newRate);

        int sampleCount() const;
        void setSampleCount(int newSampleCount);

        virtual void start();
        virtual void stop();

        virtual bool isPlaying();

    protected:
        void processData(unsigned int size, void *data) override;

    private:
        spimpl::unique_impl_ptr<SoundPrivate> impl;
};

} // namespace libscratchcpp
