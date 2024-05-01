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

/*! Returns an independent copy of the sound which is valid for as long as the original sound exists. */
std::shared_ptr<Sound> Sound::clone() const
{
    const Sound *root = impl->cloneRoot ? impl->cloneRoot : this;
    auto sound = std::make_shared<Sound>(name(), id(), dataFormat());
    sound->setRate(rate());
    sound->setSampleCount(sampleCount());
    sound->impl->cloneRoot = root;
    sound->impl->player->setVolume(impl->player->volume());

    if (root->impl->player->isLoaded()) {
        sound->impl->player->loadCopy(root->impl->player.get());
        sound->setData(dataSize(), const_cast<void *>(data()));
    }

    return sound;
}

void Sound::processData(unsigned int size, void *data)
{
    if (impl->player->isLoaded())
        return;

    if (!impl->player->load(size, data, impl->rate))
        std::cerr << "Failed to load sound " << name() << std::endl;
}
