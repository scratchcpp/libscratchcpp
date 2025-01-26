// SPDX-License-Identifier: Apache-2.0

#include "looksblocks.h"

using namespace libscratchcpp;

std::string LooksBlocks::name() const
{
    return "Looks";
}

std::string LooksBlocks::description() const
{
    return name() + " blocks";
}

Rgb LooksBlocks::color() const
{
    return rgb(153, 102, 255);
}

void LooksBlocks::registerBlocks(IEngine *engine)
{
}
