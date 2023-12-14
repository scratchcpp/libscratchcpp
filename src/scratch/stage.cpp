// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/stage.h>
#include <scratchcpp/istagehandler.h>
#include <scratchcpp/iengine.h>
#include <cassert>

#include "stage_p.h"

using namespace libscratchcpp;

/*! Constructs Stage. */
Stage::Stage() :
    Target(),
    impl(spimpl::make_unique_impl<StagePrivate>())
{
}

/*! Sets the stage interface. */
void Stage::setInterface(IStageHandler *newInterface)
{
    assert(newInterface);
    impl->iface = newInterface;
    impl->iface->init(this);
}

/*! Returns true. */
bool Stage::isStage() const
{
    return true;
}

/*! Overrides Target#setCostumeIndex(). */
void Stage::setCostumeIndex(int newCostumeIndex)
{
    Target::setCostumeIndex(newCostumeIndex);
    auto costume = costumeAt(newCostumeIndex);

    if (impl->iface)
        impl->iface->onCostumeChanged(costume.get());
}

/*! Returns the tempo. */
int Stage::tempo() const
{
    return impl->tempo;
}

/*! Sets the tempo. */
void Stage::setTempo(int newTempo)
{
    impl->tempo = newTempo;
    if (impl->iface)
        impl->iface->onTempoChanged(impl->tempo);
}

/*! Returns the video state. */
Stage::VideoState Stage::videoState() const
{
    return impl->videoState;
}

/*! Returns the video state as a string. */
std::string Stage::videoStateStr() const
{
    switch (impl->videoState) {
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
    impl->videoState = newVideoState;
    if (impl->iface)
        impl->iface->onVideoStateChanged(impl->videoState);
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
    return impl->videoTransparency;
}

/*! Sets the video transparency. */
void Stage::setVideoTransparency(int newVideoTransparency)
{
    impl->videoTransparency = newVideoTransparency;
    if (impl->iface)
        impl->iface->onVideoTransparencyChanged(impl->videoTransparency);
}

/*! Returns the text to speech language. */
const std::string &Stage::textToSpeechLanguage() const
{
    return impl->textToSpeechLanguage;
}

/*! Sets the text to speech language. */
void Stage::setTextToSpeechLanguage(const std::string &newTextToSpeechLanguage)
{
    impl->textToSpeechLanguage = newTextToSpeechLanguage;
}

/*! Overrides Target#setGraphicsEffectValue(). */
void Stage::setGraphicsEffectValue(IGraphicsEffect *effect, double value)
{
    Target::setGraphicsEffectValue(effect, value);
    IEngine *eng = engine();

    if (eng)
        eng->requestRedraw();

    if (impl->iface)
        impl->iface->onGraphicsEffectChanged(effect, value);
}

/*! Overrides Target#clearGraphicsEffects(). */
void Stage::clearGraphicsEffects()
{
    Target::clearGraphicsEffects();
    IEngine *eng = engine();

    if (eng)
        eng->requestRedraw();

    if (impl->iface)
        impl->iface->onGraphicsEffectsCleared();
}
