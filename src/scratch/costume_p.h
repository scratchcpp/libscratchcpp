// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/broadcast.h>
#include <unordered_map>

namespace libscratchcpp
{

struct CostumePrivate
{
        CostumePrivate();
        CostumePrivate(const CostumePrivate &) = delete;

        double oldBitmapResolution = 1;
        double bitmapResolution = 1;
        int rotationCenterX = 0;
        int rotationCenterY = 0;
        Broadcast broadcast;
};

} // namespace libscratchcpp
