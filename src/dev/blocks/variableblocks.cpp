// SPDX-License-Identifier: Apache-2.0

#include "variableblocks.h"

using namespace libscratchcpp;

std::string VariableBlocks::name() const
{
    return "Variables";
}

std::string VariableBlocks::description() const
{
    return "Variable blocks";
}

void VariableBlocks::registerBlocks(IEngine *engine)
{
}
