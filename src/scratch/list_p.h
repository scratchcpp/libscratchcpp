// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <scratchcpp/veque.h>
#include <scratchcpp/valuedata.h>

namespace libscratchcpp
{

class Target;
class Monitor;

struct ListPrivate
{
        ListPrivate(const std::string &name);

        std::string name;
        Target *target = nullptr;
        Monitor *monitor = nullptr;
        veque::veque<ValueData> data;
};

} // namespace libscratchcpp
