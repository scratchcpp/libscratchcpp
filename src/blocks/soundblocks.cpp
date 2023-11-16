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

    // Inputs
    engine->addInput(this, "VOLUME", VOLUME);
}

void SoundBlocks::compileChangeVolumeBy(Compiler *compiler)
{
    compiler->addInput(VOLUME);
    compiler->addFunctionCall(&changeVolumeBy);
}

unsigned int SoundBlocks::changeVolumeBy(VirtualMachine *vm)
{
    if (Target *target = vm->target())
        target->setVolume(target->volume() + vm->getInput(0, 1)->toDouble());

    return 1;
}
