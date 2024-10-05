// SPDX-License-Identifier: Apache-2.0

#include "customblocks.h"

using namespace libscratchcpp;

std::string CustomBlocks::name() const
{
    return "Custom blocks";
}

std::string CustomBlocks::description() const
{
    return name();
}

void CustomBlocks::registerBlocks(IEngine *engine)
{
}
