// SPDX-License-Identifier: Apache-2.0

#include "motionblocks.h"

using namespace libscratchcpp;

std::string MotionBlocks::name() const
{
    return "Motion";
}

std::string MotionBlocks::description() const
{
    return name() + " blocks";
}

void MotionBlocks::registerBlocks(IEngine *engine)
{
}
