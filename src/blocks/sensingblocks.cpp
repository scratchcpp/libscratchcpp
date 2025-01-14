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

void SensingBlocks::registerBlocks(IEngine *engine)
{
}
