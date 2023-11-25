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

        virtual void init(Stage *stage) = 0;

        virtual void onCostumeChanged(Costume *costume) = 0;

        virtual void onTempoChanged(int tempo) = 0;
        virtual void onVideoStateChanged(Stage::VideoState videoState) = 0;
        virtual void onVideoTransparencyChanged(int videoTransparency) = 0;
};

} // namespace libscratchcpp
