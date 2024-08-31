// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/target.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sound.h>

#include "soundblocks.h"

using namespace libscratchcpp;

// TODO: Use C++20
template<typename ContainerT, typename PredicateT>
static void erase_if(ContainerT &items, const PredicateT &predicate)
{
    for (auto it = items.begin(); it != items.end();) {
        if (predicate(*it))
            it = items.erase(it);
        else
            ++it;
    }
}

std::string SoundBlocks::name() const
{
    return "Sound";
}

void SoundBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "sound_play", &compilePlay);
    engine->addCompileFunction(this, "sound_playuntildone", &compilePlayUntilDone);
    engine->addCompileFunction(this, "sound_stopallsounds", &compileStopAllSounds);
    engine->addCompileFunction(this, "sound_seteffectto", &compileSetEffectTo);
    engine->addCompileFunction(this, "sound_changeeffectby", &compileChangeEffectBy);
    engine->addCompileFunction(this, "sound_changevolumeby", &compileChangeVolumeBy);
    engine->addCompileFunction(this, "sound_cleareffects", &compileClearEffects);
    engine->addCompileFunction(this, "sound_setvolumeto", &compileSetVolumeTo);
    engine->addCompileFunction(this, "sound_volume", &compileVolume);

    // Monitor names
    engine->addMonitorNameFunction(this, "sound_volume", &volumeMonitorName);

    // Inputs
    engine->addInput(this, "SOUND_MENU", SOUND_MENU);
    engine->addInput(this, "VALUE", VALUE);
    engine->addInput(this, "VOLUME", VOLUME);

    // Fields
    engine->addField(this, "EFFECT", EFFECT);

    // Field values
    engine->addFieldValue(this, "PITCH", PITCH);
    engine->addFieldValue(this, "PAN", PAN);
}

void SoundBlocks::onInit(IEngine *engine)
{
    m_waitingSounds.clear();
    engine->threadAboutToStop().connect([](VirtualMachine *vm) { erase_if(m_waitingSounds, [vm](const std::pair<Sound *, VirtualMachine *> &pair) { return pair.second == vm; }); });
}

bool SoundBlocks::compilePlayCommon(Compiler *compiler, bool untilDone, bool *byIndex)
{
    Target *target = compiler->target();
    assert(target);

    if (!target)
        return false;

    Input *input = compiler->input(SOUND_MENU);

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        int index = target->findSound(value);

        if (index == -1) {
            Value v(value);

            if (v.isValidNumber() && !v.isInfinity() && !v.isNegativeInfinity() && !(v.isString() && v.toString().empty())) {
                compiler->addConstValue(v.toLong() - 1);
                compiler->addFunctionCall(untilDone ? &playByIndexUntilDone : &playByIndex);

                if (byIndex)
                    *byIndex = true;

                return true;
            }
        } else {
            compiler->addConstValue(index);
            compiler->addFunctionCall(untilDone ? &playByIndexUntilDone : &playByIndex);

            if (byIndex)
                *byIndex = true;

            return true;
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(untilDone ? &playUntilDone : &play);

        if (byIndex)
            *byIndex = false;

        return true;
    }

    return false;
}

void SoundBlocks::compilePlay(Compiler *compiler)
{
    compilePlayCommon(compiler, false);
}

void SoundBlocks::compilePlayUntilDone(Compiler *compiler)
{
    bool byIndex = false;

    if (compilePlayCommon(compiler, true, &byIndex))
        compiler->addFunctionCall(byIndex ? &checkSoundByIndex : &checkSound);
}

void SoundBlocks::compileStopAllSounds(Compiler *compiler)
{
    compiler->addFunctionCall(&stopAllSounds);
}

void SoundBlocks::compileSetEffectTo(Compiler *compiler)
{
    compiler->addInput(VALUE);
    int option = compiler->field(EFFECT)->specialValueId();

    switch (option) {
        case PITCH:
            compiler->addFunctionCall(&setPitchEffectTo);
            break;

        case PAN:
            compiler->addFunctionCall(&setPanEffectTo);
            break;

        default:
            assert(false);
            break;
    }
}

void SoundBlocks::compileChangeEffectBy(Compiler *compiler)
{
    compiler->addInput(VALUE);
    int option = compiler->field(EFFECT)->specialValueId();

    switch (option) {
        case PITCH:
            compiler->addFunctionCall(&changePitchEffectBy);
            break;

        case PAN:
            compiler->addFunctionCall(&changePanEffectBy);
            break;

        default:
            assert(false);
            break;
    }
}

void SoundBlocks::compileClearEffects(Compiler *compiler)
{
    compiler->addFunctionCall(&clearEffects);
}

void SoundBlocks::compileChangeVolumeBy(Compiler *compiler)
{
    compiler->addInput(VOLUME);
    compiler->addFunctionCall(&changeVolumeBy);
}

void SoundBlocks::compileSetVolumeTo(Compiler *compiler)
{
    compiler->addInput(VOLUME);
    compiler->addFunctionCall(&setVolumeTo);
}

void SoundBlocks::compileVolume(Compiler *compiler)
{
    compiler->addFunctionCall(&volume);
}

const std::string &SoundBlocks::volumeMonitorName(Block *block)
{
    static const std::string name = "volume";
    return name;
}

Sound *SoundBlocks::getSoundByIndex(Target *target, long index)
{
    long soundCount = target->sounds().size();

    if (index < 0 || index >= soundCount) {
        if (index < 0)
            index = std::fmod(soundCount + std::fmod(index, -soundCount), soundCount);
        else
            index = std::fmod(index, soundCount);
    }

    return target->soundAt(index).get();
}

Sound *SoundBlocks::playCommon(VirtualMachine *vm)
{
    Target *target = vm->target();
    assert(target);
    const Value *name = vm->getInput(0, 1);

    if (target) {
        Sound *sound = target->soundAt(target->findSound(name->toString())).get();

        if (sound) {
            sound->start();
            return sound;
        }

        if (name->isValidNumber() && !name->isInfinity() && !name->isNegativeInfinity() && !(name->isString() && name->toString().empty())) {
            sound = getSoundByIndex(target, name->toLong() - 1);

            if (sound) {
                sound->start();
                return sound;
            }
        }
    }

    return nullptr;
}

Sound *SoundBlocks::playByIndexCommon(VirtualMachine *vm)
{
    Target *target = vm->target();
    assert(target);

    if (target) {
        Sound *sound = getSoundByIndex(target, vm->getInput(0, 1)->toInt());

        if (sound) {
            sound->start();
            return sound;
        }
    }

    return nullptr;
}

unsigned int SoundBlocks::play(VirtualMachine *vm)
{
    Sound *sound = playCommon(vm);

    if (sound)
        m_waitingSounds.erase(sound);

    return 1;
}

unsigned int SoundBlocks::playByIndex(VirtualMachine *vm)
{
    Sound *sound = playByIndexCommon(vm);

    if (sound)
        m_waitingSounds.erase(sound);

    return 1;
}

unsigned int SoundBlocks::playUntilDone(VirtualMachine *vm)
{
    Sound *sound = playCommon(vm);

    if (sound)
        m_waitingSounds[sound] = vm;

    return 0; // leave the register for checkSound()
}

unsigned int SoundBlocks::playByIndexUntilDone(VirtualMachine *vm)
{
    Sound *sound = playByIndexCommon(vm);

    if (sound)
        m_waitingSounds[sound] = vm;

    return 0; // leave the register for checkSoundByIndex()
}

unsigned int SoundBlocks::checkSound(VirtualMachine *vm)
{
    Target *target = vm->target();
    assert(target);
    const Value *name = vm->getInput(0, 1);

    if (target) {
        Sound *sound = target->soundAt(target->findSound(name->toString())).get();

        if (!sound && name->isValidNumber() && !name->isInfinity() && !name->isNegativeInfinity() && !(name->isString() && name->toString().empty()))
            sound = getSoundByIndex(target, name->toLong() - 1);

        if (sound) {
            auto it = m_waitingSounds.find(sound);

            if (it != m_waitingSounds.cend() && it->second == vm) {
                if (sound->isPlaying())
                    vm->stop(true, true, true);
                else
                    m_waitingSounds.erase(sound);
            }
        }
    }

    return 1;
}

unsigned int SoundBlocks::checkSoundByIndex(VirtualMachine *vm)
{
    Target *target = vm->target();
    assert(target);

    if (target) {
        auto sound = getSoundByIndex(target, vm->getInput(0, 1)->toInt());

        if (sound) {
            auto it = m_waitingSounds.find(sound);

            if (it != m_waitingSounds.cend() && it->second == vm) {
                if (sound->isPlaying())
                    vm->stop(true, true, true);
                else
                    m_waitingSounds.erase(sound);
            }
        }
    }

    return 1;
}

unsigned int SoundBlocks::stopAllSounds(VirtualMachine *vm)
{
    vm->engine()->stopSounds();
    m_waitingSounds.clear();
    return 0;
}

unsigned int SoundBlocks::setPitchEffectTo(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->setSoundEffectValue(Sound::Effect::Pitch, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int SoundBlocks::setPanEffectTo(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->setSoundEffectValue(Sound::Effect::Pan, vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int SoundBlocks::changePitchEffectBy(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->setSoundEffectValue(Sound::Effect::Pitch, target->soundEffectValue(Sound::Effect::Pitch) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int SoundBlocks::changePanEffectBy(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->setSoundEffectValue(Sound::Effect::Pan, target->soundEffectValue(Sound::Effect::Pan) + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int SoundBlocks::clearEffects(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->clearSoundEffects();

    return 0;
}

unsigned int SoundBlocks::changeVolumeBy(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->setVolume(target->volume() + vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int SoundBlocks::setVolumeTo(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->setVolume(vm->getInput(0, 1)->toDouble());

    return 1;
}

unsigned int SoundBlocks::volume(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        vm->addReturnValue(target->volume());
    else
        vm->addReturnValue(0);

    return 0;
}
