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

        virtual void onValueChanged(const Value &value) = 0;
        virtual void onXChanged(int x) = 0;
        virtual void onYChanged(int y) = 0;
        virtual void onVisibleChanged(bool visible) = 0;
};

} // namespace libscratchcpp
