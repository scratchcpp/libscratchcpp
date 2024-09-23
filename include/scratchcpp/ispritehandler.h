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

        /*! Called when the sprite is deinitialized (if it is a clone). */
        virtual void deinitClone() = 0;

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

        /*! Called when the sprite position changes. */
        virtual void onMoved(double oldX, double oldY, double newX, double newY) = 0;

        /*! Called when the size changes. */
        virtual void onSizeChanged(double size) = 0;

        /*! Called when the direction changes. */
        virtual void onDirectionChanged(double direction) = 0;

        /*! Called when the rotation style changes. */
        virtual void onRotationStyleChanged(Sprite::RotationStyle rotationStyle) = 0;

        /*! Called when the layer order changes. */
        virtual void onLayerOrderChanged(int layerOrder) = 0;

        /*!
         * Called when the value of the given graphics effect changes.
         * \note This method isn't called when all effects are cleared, use onGraphicsEffectsCleared() for this.
         */
        virtual void onGraphicsEffectChanged(IGraphicsEffect *effect, double value) = 0;

        /*! Called when all graphics effects are cleared. */
        virtual void onGraphicsEffectsCleared() = 0;

        /*! Used to get the current costume width. */
        virtual int costumeWidth() const = 0;

        /*! Used to get the current costume height. */
        virtual int costumeHeight() const = 0;

        /*!
         * Used to get the bounding rectangle of the sprite.
         * \note The rectangle must be relative to the stage, so make sure to use the sprite's coordinates.
         */
        virtual Rect boundingRect() const = 0;

        /*!
         * Used to get a less accurate bounding rectangle of the sprite
         * which is calculated by transforming the costume rectangle.
         * \note The rectangle must be relative to the stage, so make sure to use the sprite's coordinates.
         */
        virtual Rect fastBoundingRect() const = 0;

        /*! Used to check whether the sprite touches any of the given sprite clones. */
        virtual bool touchingClones(const std::vector<Sprite *> &clones) const = 0;

        /*! Used to check whether the sprite touches the given point (in Scratch coordinates). */
        virtual bool touchingPoint(double x, double y) const = 0;

        /*! Used to check whether the sprite touches the given color. */
        virtual bool touchingColor(const Value &color) const = 0;

        /*! Used to check whether the mask part of the sprite touches the given color. */
        virtual bool touchingColor(const Value &color, const Value &mask) const = 0;
};

} // namespace libscratchcpp
