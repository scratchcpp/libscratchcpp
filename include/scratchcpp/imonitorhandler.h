// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

class Monitor;
class Value;

class LIBSCRATCHCPP_EXPORT IMonitorHandler
{
    public:
        virtual ~IMonitorHandler() { }

        virtual void init(Monitor *monitor) = 0;

        virtual void onValueChanged(const VirtualMachine *vm) = 0;
        virtual void onVisibleChanged(bool visible) = 0;
};

} // namespace libscratchcpp
