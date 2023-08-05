// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "target.h"

namespace libscratchcpp
{

class IStageHandler;

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

        void setInterface(IStageHandler *newInterface);

        bool isStage() const override;
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

    protected:
        void setCostumeData(const char *data) override;

    private:
        IStageHandler *m_iface = nullptr;
        int m_tempo;
        VideoState m_videoState = VideoState::Off;
        int m_videoTransparency = 50;
        std::string m_textToSpeechLanguage;
};

} // namespace libscratchcpp
