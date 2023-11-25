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

        /*! Called when the interface is set on a sprite. */
        virtual void init(Sprite *sprite) = 0;

        /*! Called when the sprite clones. */
        virtual void onCloned(Sprite *clone) = 0;

        /*! Called when the costume changes. */
        virtual void onCostumeChanged(Costume *costume) = 0;

        /*! Called when the visibility changes. */
        virtual void onVisibleChanged(bool visible) = 0;

        /*! Called when the X-coordinate changes. */
        virtual void onXChanged(double x) = 0;

        /*! Called when the Y-coordinate changes. */
        virtual void onYChanged(double y) = 0;

        /*! Called when the size changes. */
        virtual void onSizeChanged(double size) = 0;

        /*! Called when the direction changes. */
        virtual void onDirectionChanged(double direction) = 0;

        /*! Called when the rotation style changes. */
        virtual void onRotationStyleChanged(Sprite::RotationStyle rotationStyle) = 0;
};

} // namespace libscratchcpp
