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

        int rate() const;
        void setRate(int newRate);

        int sampleCount() const;
        void setSampleCount(int newSampleCount);

    private:
        spimpl::unique_impl_ptr<SoundPrivate> impl;
};

} // namespace libscratchcpp
