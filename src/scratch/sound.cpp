// SPDX-License-Identifier: Apache-2.0

#include "sound.h"

using namespace libscratchcpp;

/*! Constructs Sound. */
Sound::Sound(std::string name, std::string id, std::string format) :
    Asset(name, id, format)
{
}

/*! Returns the sampling rate of the sound in Hertz. */
int Sound::rate() const
{
    return m_rate;
}

/*! Sets the sampling rate of the sound in Hertz. */
void Sound::setRate(int newRate)
{
    m_rate = newRate;
}

/*! Returns the number of samples. */
int Sound::sampleCount() const
{
    return m_sampleCount;
}

/*! Sets the number of samples. */
void Sound::setSampleCount(int newSampleCount)
{
    m_sampleCount = newSampleCount;
}
