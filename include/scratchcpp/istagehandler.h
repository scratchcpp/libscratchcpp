// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "stage.h"

namespace libscratchcpp
{

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
};

} // namespace libscratchcpp
