// SPDX-License-Identifier: Apache-2.0

#include "operatorblocks.h"

using namespace libscratchcpp;

std::string OperatorBlocks::name() const
{
    return "Operators";
}

std::string OperatorBlocks::description() const
{
    return "Operator blocks";
}

void OperatorBlocks::registerBlocks(IEngine *engine)
{
}
