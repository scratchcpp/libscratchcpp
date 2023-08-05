// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

struct ListPrivate
{
        ListPrivate(const std::string &name);

        std::string name;
};

} // namespace libscratchcpp
