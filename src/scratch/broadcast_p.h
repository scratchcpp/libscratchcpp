// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

struct BroadcastPrivate
{
        BroadcastPrivate(const std::string &name, bool isBackdropBroadcast);
        BroadcastPrivate(const BroadcastPrivate &) = delete;

        std::string name;
        bool isBackdropBroadcast = false;
};

} // namespace libscratchcpp
