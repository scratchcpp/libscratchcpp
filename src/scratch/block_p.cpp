// SPDX-License-Identifier: Apache-2.0

#include "block_p.h"

using namespace libscratchcpp;

BlockPrivate::BlockPrivate(const std::string &opcode, bool isMonitorBlock) :
    opcode(opcode),
    isMonitorBlock(isMonitorBlock)
{
}
