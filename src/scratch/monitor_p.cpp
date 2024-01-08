// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/block.h>

#include "monitor_p.h"

using namespace libscratchcpp;

MonitorPrivate::MonitorPrivate(const std::string &opcode) :
    block(std::make_shared<Block>("", opcode))
{
}
