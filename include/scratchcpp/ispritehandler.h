// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "sprite.h"

namespace libscratchcpp
{

/*! \brief The ISpriteHandler class provides a sprite interface for Scratch project players. */
class LIBSCRATCHCPP_EXPORT ISpriteHandler
{
    public:
        virtual ~ISpriteHandler() { }

        virtual void init(Sprite *sprite) = 0;

        virtual void onCloned(Sprite *clone) = 0;

        virtual void onCostumeChanged(Costume *costume) = 0;

        virtual void onVisibleChanged(bool visible) = 0;
        virtual void onXChanged(double x) = 0;
        virtual void onYChanged(double y) = 0;
        virtual void onSizeChanged(double size) = 0;
        virtual void onDirectionChanged(double direction) = 0;
        virtual void onRotationStyleChanged(Sprite::RotationStyle rotationStyle) = 0;
};

} // namespace libscratchcpp
