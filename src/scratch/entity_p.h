// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

struct EntityPrivate
{
        EntityPrivate(const std::string &id);

        std::string id;
};

} // namespace libscratchcpp
