// SPDX-License-Identifier: Apache-2.0

#include "sound_p.h"
#include "audio/audioplayerfactory.h"

using namespace libscratchcpp;

IAudioPlayerFactory *SoundPrivate::playerFactory = nullptr;

SoundPrivate::SoundPrivate()
{
    // NOTE: playerFactory must be initialized in the constructor to avoid static initialization order fiasco
    if (!playerFactory)
        playerFactory = AudioPlayerFactory::instance().get();

    player = playerFactory->create();
}
