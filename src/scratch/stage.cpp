// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <scratchcpp/istagehandler.h>

#include "stage.h"

using namespace libscratchcpp;

/*! Sets the stage interface. */
void Stage::setInterface(IStageHandler *newInterface)
{
    assert(newInterface);
    m_iface = newInterface;
    m_iface->onStageChanged(this);
}

/*! Returns true. */
bool Stage::isStage() const
{
    return true;
}

/*! Returns the tempo. */
int Stage::tempo() const
{
    return m_tempo;
}

/*! Sets the tempo. */
void Stage::setTempo(int newTempo)
{
    m_tempo = newTempo;
    if (m_iface)
        m_iface->onTempoChanged(m_tempo);
}

/*! Returns the video state. */
Stage::VideoState Stage::videoState() const
{
    return m_videoState;
}

/*! Returns the video state as a string. */
std::string Stage::videoStateStr() const
{
    switch (m_videoState) {
        case VideoState::Off:
            return "off";
        case VideoState::On:
            return "on";
        case VideoState::OnFlipped:
            return "on-flipped";
    }
    return "off";
}

/*! Sets the video state. */
void Stage::setVideoState(VideoState newVideoState)
{
    m_videoState = newVideoState;
    if (m_iface)
        m_iface->onVideoStateChanged(m_videoState);
}

/*! \copydoc setVideoState() */
void Stage::setVideoState(const std::string &newVideoState)
{
    if (newVideoState == "on")
        setVideoState(VideoState::On);
    else if (newVideoState == "on-flipped")
        setVideoState(VideoState::OnFlipped);
    else if (newVideoState == "off")
        setVideoState(VideoState::Off);
}

/*! \copydoc setVideoState() */
void Stage::setVideoState(const char *newVideoState)
{
    setVideoState(std::string(newVideoState));
}

/*! Returns the video transparency. */
int Stage::videoTransparency() const
{
    return m_videoTransparency;
}

/*! Sets the video transparency. */
void Stage::setVideoTransparency(int newVideoTransparency)
{
    m_videoTransparency = newVideoTransparency;
    if (m_iface)
        m_iface->onVideoTransparencyChanged(m_videoTransparency);
}

/*! Returns the text to speech language. */
const std::string &Stage::textToSpeechLanguage() const
{
    return m_textToSpeechLanguage;
}

/*! Sets the text to speech language. */
void Stage::setTextToSpeechLanguage(const std::string &newTextToSpeechLanguage)
{
    m_textToSpeechLanguage = newTextToSpeechLanguage;
}

void Stage::setCostumeData(const char *data)
{
    if (m_iface)
        m_iface->onCostumeChanged(data);
}
