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
        IStageHandler *getInterface() const; // NOTE: This can't be called interface because of... Microsoft... (it wouldn't compile on Windows)

        bool isStage() const override;

        void setCostumeIndex(int newCostumeIndex) override;

        int currentCostumeWidth() const override;
        int currentCostumeHeight() const override;

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

        Rect boundingRect() const override;
        Rect fastBoundingRect() const override;

        bool touchingPoint(double x, double y) const override;
        bool touchingColor(const Value &color) const override;

        void setGraphicsEffectValue(IGraphicsEffect *effect, double value) override;

        void clearGraphicsEffects() override;

        virtual void setBubbleType(Target::BubbleType type) override;
        virtual void setBubbleText(const std::string &text) override;

    private:
        bool touchingClones(const std::vector<Sprite *> &clones) const override;

        spimpl::unique_impl_ptr<StagePrivate> impl;
};

} // namespace libscratchcpp
