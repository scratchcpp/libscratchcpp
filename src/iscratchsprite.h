// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "internal/iscratchtarget.h"
#include "scratch/sprite.h"

namespace libscratchcpp
{

/*! \brief The IScratchSprite class provides a sprite interface for Scratch project players. */
class LIBSCRATCHCPP_EXPORT IScratchSprite : public IScratchTarget
{
    public:
        virtual ~IScratchSprite() { }
        virtual Sprite *target() const override { return dynamic_cast<Sprite *>(IScratchTarget::target()); };

        virtual void setVisible(bool visible) = 0;
        virtual void setX(double x) = 0;
        virtual void setY(double y) = 0;
        virtual void setSize(double size) = 0;
        virtual void setDirection(double direction) = 0;
        virtual void setRotationStyle(Sprite::RotationStyle rotationStyle) = 0;
};

} // namespace libscratchcpp
