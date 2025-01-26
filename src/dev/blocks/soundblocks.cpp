// SPDX-License-Identifier: Apache-2.0

#include "soundblocks.h"

using namespace libscratchcpp;

std::string SoundBlocks::name() const
{
    return "Sound";
}

std::string SoundBlocks::description() const
{
    return name() + " blocks";
}

Rgb SoundBlocks::color() const
{
    return rgb(207, 99, 207);
}

void SoundBlocks::registerBlocks(IEngine *engine)
{
}
