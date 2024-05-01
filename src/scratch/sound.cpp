// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/sound.h>
#include <scratchcpp/sprite.h>
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
    // Stop sounds in clones (#538)
    stopCloneSounds();
    impl->player->start();
}

/*! Stops the playback of the sound. */
void Sound::stop()
{
    // Stop sounds in clones (#538)
    stopCloneSounds();
    impl->player->stop();
}

/*! Returns true if the sound is being played. */
bool Sound::isPlaying()
{
    return impl->player->isPlaying();
}

/*! Returns the sprite or stage this variable belongs to. */
Target *Sound::target() const
{
    return impl->target;
}

/*! Sets the sprite or stage this variable belongs to. */
void Sound::setTarget(Target *target)
{
    impl->target = target;
}

/*! Returns an independent copy of the sound which is valid for as long as the original sound exists. */
std::shared_ptr<Sound> Sound::clone() const
{
    auto sound = std::make_shared<Sound>(name(), id(), dataFormat());
    sound->setRate(rate());
    sound->setSampleCount(sampleCount());
    sound->impl->player->setVolume(impl->player->volume());

    if (impl->player->isLoaded()) {
        sound->impl->player->loadCopy(impl->player.get());
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

void Sound::stopCloneSounds()
{
    if (impl->target && !impl->target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(impl->target);

        if (sprite->isClone())
            sprite = sprite->cloneSprite();

        // Stop the sound in the sprite (clone root)
        auto sound = sprite->soundAt(sprite->findSound(name()));

        if (sound && sound.get() != this)
            sound->impl->player->stop();

        // Stop the sound in clones
        const auto &clones = sprite->clones();

        for (auto clone : clones) {
            auto sound = clone->soundAt(clone->findSound(name()));
            assert(sound);

            if (sound && sound.get() != this)
                sound->impl->player->stop();
        }
    }
}
