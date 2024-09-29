// SPDX-License-Identifier: Apache-2.0

#include "eventblocks.h"

using namespace libscratchcpp;

std::string EventBlocks::name() const
{
    return "Events";
}

std::string libscratchcpp::EventBlocks::description() const
{
    return "Event blocks";
}

void EventBlocks::registerBlocks(IEngine *engine)
{
}
