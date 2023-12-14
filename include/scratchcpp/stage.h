// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "target.h"

namespace libscratchcpp
{

class IStageHandler;
class StagePrivate;

/*! \brief The Stage class is the project stage. */
class LIBSCRATCHCPP_EXPORT Stage : public Target
{
    public:
        enum class VideoState
        {
            On,
            Off,
            OnFlipped
        };

        Stage();
        Stage(const Stage &) = delete;

        void setInterface(IStageHandler *newInterface);

        bool isStage() const override;

        void setCostumeIndex(int newCostumeIndex) override;

        int tempo() const;
        void setTempo(int newTempo);

        VideoState videoState() const;
        std::string videoStateStr() const;
        void setVideoState(VideoState newVideoState);
        void setVideoState(const std::string &newVideoState);
        void setVideoState(const char *newVideoState);

        int videoTransparency() const;
        void setVideoTransparency(int newVideoTransparency);

        const std::string &textToSpeechLanguage() const;
        void setTextToSpeechLanguage(const std::string &newTextToSpeechLanguage);

        void setGraphicsEffectValue(IGraphicsEffect *effect, double value) override;

        void clearGraphicsEffects() override;

    private:
        spimpl::unique_impl_ptr<StagePrivate> impl;
};

} // namespace libscratchcpp
