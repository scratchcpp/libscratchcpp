// SPDX-License-Identifier: Apache-2.0

#include "controlblocks.h"

using namespace libscratchcpp;

std::string ControlBlocks::name() const
{
    return "Control";
}

std::string ControlBlocks::description() const
{
    return name() + " blocks";
}

void ControlBlocks::registerBlocks(IEngine *engine)
{
}
