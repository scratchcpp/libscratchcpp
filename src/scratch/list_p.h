// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

class Target;

struct ListPrivate
{
        ListPrivate(const std::string &name);

        std::string name;
        Target *target = nullptr;
};

} // namespace libscratchcpp
