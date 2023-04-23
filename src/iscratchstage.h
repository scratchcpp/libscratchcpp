// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "internal/iscratchtarget.h"
#include "scratch/stage.h"

namespace libscratchcpp
{

/*! \brief The IScratchStage class provides a stage interface for Scratch project players. */
class LIBSCRATCHCPP_EXPORT IScratchStage : public IScratchTarget
{
    public:
        virtual ~IScratchStage() { }
        virtual Stage *target() const override { return dynamic_cast<Stage *>(IScratchTarget::target()); };

        virtual void setTempo(int tempo) = 0;
        virtual void setVideoState(Stage::VideoState videoState) = 0;
        virtual void setVideoTransparency(int videoTransparency) = 0;
};

} // namespace libscratchcpp
