// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/sound.h>
#include <iostream>

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

/*! Sets the volume percentage of the sound. */
void Sound::setVolume(double volume)
{
    impl->player->setVolume(volume / 100);
}

/*! Starts the playback of the sound. */
void Sound::start()
{
    impl->player->start();
}

/*! Stops the playback of the sound. */
void Sound::stop()
{
    impl->player->stop();
}

/*! Returns true if the sound is being played. */
bool Sound::isPlaying()
{
    return impl->player->isPlaying();
}

void Sound::processData(unsigned int size, void *data)
{
    if (!impl->player->load(size, data, impl->rate))
        std::cerr << "Failed to load sound " << name() << std::endl;
}
