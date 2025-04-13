// SPDX-License-Identifier: Apache-2.0

#include "audiooutput.h"

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
#include "internal/audioplayer.h"
#else
#include "internal/audioplayerstub.h"
#endif

using namespace libscratchcpp;

static std::shared_ptr<IAudioOutput> INSTANCE = std::make_shared<AudioOutput>();

AudioOutput::AudioOutput()
{
}

std::shared_ptr<IAudioOutput> AudioOutput::instance()
{
    return INSTANCE;
}

std::shared_ptr<IAudioPlayer> AudioOutput::createAudioPlayer() const
{
#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
    return std::make_shared<AudioPlayer>();
#else
    return std::make_shared<AudioPlayerStub>();
#endif
}
