// SPDX-License-Identifier: Apache-2.0

#include "sensingblocks.h"

using namespace libscratchcpp;

std::string SensingBlocks::name() const
{
    return "Sensing";
}

std::string SensingBlocks::description() const
{
    return name() + " blocks";
}

Rgb SensingBlocks::color() const
{
    return rgb(92, 177, 214);
}

void SensingBlocks::registerBlocks(IEngine *engine)
{
}
