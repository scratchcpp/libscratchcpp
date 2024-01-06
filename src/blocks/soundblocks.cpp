// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/target.h>
#include <scratchcpp/input.h>
#include <scratchcpp/sound.h>

#include "soundblocks.h"

using namespace libscratchcpp;

std::string SoundBlocks::name() const
{
    return "Sound";
}

void SoundBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "sound_play", &compilePlay);
    engine->addCompileFunction(this, "sound_changevolumeby", &compileChangeVolumeBy);
    engine->addCompileFunction(this, "sound_setvolumeto", &compileSetVolumeTo);
    engine->addCompileFunction(this, "sound_volume", &compileVolume);

    // Inputs
    engine->addInput(this, "SOUND_MENU", SOUND_MENU);
    engine->addInput(this, "VOLUME", VOLUME);
}

void SoundBlocks::compilePlay(Compiler *compiler)
{
    Target *target = compiler->target();
    assert(target);

    if (!target)
        return;

    Input *input = compiler->input(SOUND_MENU);

    if (input->type() != Input::Type::ObscuredShadow) {
        assert(input->pointsToDropdownMenu());
        std::string value = input->selectedMenuItem();

        int index = target->findSound(value);

        if (index == -1) {
            Value v(value);

            if (v.type() == Value::Type::Integer) {
                compiler->addConstValue(v.toLong() - 1);
                compiler->addFunctionCall(&playByIndex);
            }
        } else {
            compiler->addConstValue(index);
            compiler->addFunctionCall(&playByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&play);
    }
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

unsigned int SoundBlocks::play(VirtualMachine *vm)
{
    Target *target = vm->target();
    assert(target);
    const Value *name = vm->getInput(0, 1);

    if (target) {
        Sound *sound = target->soundAt(target->findSound(name->toString())).get();

        if (sound)
            sound->start();
        else if (name->type() == Value::Type::Integer) {
            sound = getSoundByIndex(target, name->toLong() - 1);

            if (sound)
                sound->start();
        }
    }

    return 1;
}

unsigned int SoundBlocks::playByIndex(VirtualMachine *vm)
{
    Target *target = vm->target();
    assert(target);

    if (target) {
        auto sound = getSoundByIndex(target, vm->getInput(0, 1)->toInt());

        if (sound)
            sound->start();
    }
    return 1;
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
