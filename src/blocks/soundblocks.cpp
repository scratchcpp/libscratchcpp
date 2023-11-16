// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/target.h>

#include "soundblocks.h"

using namespace libscratchcpp;

std::string SoundBlocks::name() const
{
    return "Sound";
}

void SoundBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "sound_changevolumeby", &compileChangeVolumeBy);
    engine->addCompileFunction(this, "sound_setvolumeto", &compileSetVolumeTo);
    engine->addCompileFunction(this, "sound_volume", &compileVolume);

    // Inputs
    engine->addInput(this, "VOLUME", VOLUME);
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
