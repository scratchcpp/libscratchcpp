// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/sound.h>

#include "sound_p.h"

using namespace libscratchcpp;

/*! Constructs Sound. */
Sound::Sound(const std::string &name, const std::string &id, const std::string &format) :
    Asset(name, id, format),
    impl(spimpl::make_unique_impl<SoundPrivate>())
{
}

/*! Returns the sampling rate of the sound in Hertz. */
int Sound::rate() const
{
    return impl->rate;
}

/*! Sets the sampling rate of the sound in Hertz. */
void Sound::setRate(int newRate)
{
    impl->rate = newRate;
}

/*! Returns the number of samples. */
int Sound::sampleCount() const
{
    return impl->sampleCount;
}

/*! Sets the number of samples. */
void Sound::setSampleCount(int newSampleCount)
{
    impl->sampleCount = newSampleCount;
}
