// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "stage.h"

namespace libscratchcpp
{

class Sprite;

/*! \brief The IStageHandler class provides a stage interface for Scratch project players. */
class LIBSCRATCHCPP_EXPORT IStageHandler
{
    public:
        virtual ~IStageHandler() { }

        /*! Called when the interface is set on a stage. */
        virtual void init(Stage *stage) = 0;

        /*! Called when the costume changes. */
        virtual void onCostumeChanged(Costume *costume) = 0;

        /*! Called when the tempo changes. */
        virtual void onTempoChanged(int tempo) = 0;

        /*! Called when the video state changes. */
        virtual void onVideoStateChanged(Stage::VideoState videoState) = 0;

        /*! Called when the video transparency changes. */
        virtual void onVideoTransparencyChanged(int videoTransparency) = 0;

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

        /*! Used to get the bounding rectangle of the stage. */
        virtual Rect boundingRect() const = 0;

        /*!
         * Used to get a less accurate bounding rectangle of the stage
         * which is calculated by transforming the costume rectangle.
         */
        virtual Rect fastBoundingRect() const = 0;

        /*! Used to check whether the stage touches any of the given sprite clones. */
        virtual bool touchingClones(const std::vector<Sprite *> &clones) const = 0;

        /*! Used to check whether the stage touches the given point (in Scratch coordinates). */
        virtual bool touchingPoint(double x, double y) const = 0;

        /*! Used to check whether the stage touches the given color. */
        virtual bool touchingColor(const Value &color) const = 0;

        /*! Used to check whether the mask part of the stage touches the given color. */
        virtual bool touchingColor(const Value &color, const Value &mask) const = 0;
};

} // namespace libscratchcpp
