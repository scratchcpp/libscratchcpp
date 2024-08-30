// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/sound.h>
#include <scratchcpp/sprite.h>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cmath>

#include "sound_p.h"

using namespace libscratchcpp;

static std::unordered_map<Sound::Effect, std::pair<double, double>> EFFECT_RANGE = {
    { Sound::Effect::Pitch, { -360, 360 } } // -3 to 3 octaves
};

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

/*! Sets the value of the given sound effect. */
void Sound::setEffect(Effect effect, double value)
{
    auto it = EFFECT_RANGE.find(effect);

    if (it == EFFECT_RANGE.cend())
        return;

    value = std::clamp(value, it->second.first, it->second.second);

    switch (effect) {
        case Effect::Pitch:
            // Convert from linear
            const double root = std::pow(2, 1 / 12.0);
            impl->player->setPitch(std::pow(root, value / 10));
            break;
    }
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
bool Sound::isPlaying() const
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
    const Sound *root = impl->cloneRoot ? impl->cloneRoot : this;
    auto sound = std::make_shared<Sound>(name(), id(), dataFormat());
    sound->setRate(rate());
    sound->setSampleCount(sampleCount());
    sound->impl->cloneRoot = root;
    sound->impl->player->setVolume(impl->player->volume());
    sound->impl->player->setPitch(impl->player->pitch());

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

void Sound::stopCloneSounds()
{
    if (impl->target && !impl->target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(impl->target);

        if (sprite->isClone())
            sprite = sprite->cloneSprite();

        // Stop the sound in the sprite (clone root)
        const Sound *root = impl->cloneRoot ? impl->cloneRoot : this;
        assert(root == sprite->soundAt(sprite->findSound(name())).get());

        if (root != this)
            root->impl->player->stop();

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
