// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

class Monitor;

class LIBSCRATCHCPP_EXPORT IMonitorHandler
{
    public:
        virtual ~IMonitorHandler() { }

        virtual void init(Monitor *) = 0;
};

} // namespace libscratchcpp
