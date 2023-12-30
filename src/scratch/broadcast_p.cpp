// SPDX-License-Identifier: Apache-2.0

#include "broadcast_p.h"

namespace libscratchcpp
{

BroadcastPrivate::BroadcastPrivate(const std::string &name, bool isBackdropBroadcast) :
    name(name),
    isBackdropBroadcast(isBackdropBroadcast)
{
}

} // namespace libscratchcpp
