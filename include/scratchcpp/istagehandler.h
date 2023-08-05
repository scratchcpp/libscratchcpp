// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

// TODO: Remove this
#include "scratch/stage.h"

namespace libscratchcpp
{

class Stage;

/*! \brief The IStageHandler class provides a stage interface for Scratch project players. */
class LIBSCRATCHCPP_EXPORT IStageHandler
{
    public:
        virtual ~IStageHandler() { }

        virtual void onStageChanged(Stage *stage) = 0;

        virtual void onCostumeChanged(const char *data) = 0;

        virtual void onTempoChanged(int tempo) = 0;
        virtual void onVideoStateChanged(Stage::VideoState videoState) = 0;
        virtual void onVideoTransparencyChanged(int videoTransparency) = 0;
};

} // namespace libscratchcpp
