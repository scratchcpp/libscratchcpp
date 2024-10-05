// SPDX-License-Identifier: Apache-2.0

#include "listblocks.h"

using namespace libscratchcpp;

std::string ListBlocks::name() const
{
    return "Lists";
}

std::string ListBlocks::description() const
{
    return "List blocks";
}

void ListBlocks::registerBlocks(IEngine *engine)
{
}
