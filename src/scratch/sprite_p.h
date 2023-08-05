// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/sprite.h>

namespace libscratchcpp
{

struct SpritePrivate
{
        SpritePrivate();
        SpritePrivate(const SpritePrivate &) = delete;

        void setCostumeData(const char *data);

        ISpriteHandler *iface = nullptr;
        bool visible = true;
        double x = 0;
        double y = 0;
        double size = 100;
        double direction = 90;
        bool draggable = false;
        Sprite::RotationStyle rotationStyle = Sprite::RotationStyle::AllAround;
};

} // namespace libscratchcpp
