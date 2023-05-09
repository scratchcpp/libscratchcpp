// SPDX-License-Identifier: Apache-2.0

#include "eventblocks.h"

using namespace libscratchcpp;

EventBlocks::EventBlocks()
{
    addHatBlock("event_whenflagclicked");
}

std::string EventBlocks::name() const
{
    return "Events";
}
