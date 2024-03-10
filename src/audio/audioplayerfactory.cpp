// SPDX-License-Identifier: Apache-2.0

#include "audioplayerfactory.h"

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
#include "internal/audioplayer.h"
#else
#include "internal/audioplayerstub.h"
#endif

using namespace libscratchcpp;

std::shared_ptr<AudioPlayerFactory> AudioPlayerFactory::m_instance = std::make_shared<AudioPlayerFactory>();

AudioPlayerFactory::AudioPlayerFactory()
{
}

std::shared_ptr<AudioPlayerFactory> AudioPlayerFactory::instance()
{
    return m_instance;
}

std::shared_ptr<IAudioPlayer> AudioPlayerFactory::create() const
{
#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
    return std::make_shared<AudioPlayer>();
#else
    return std::make_shared<AudioPlayerStub>();
#endif
}
