// SPDX-License-Identifier: Apache-2.0

#include "sound_p.h"
#include "audio/audiooutput.h"

using namespace libscratchcpp;

IAudioOutput *SoundPrivate::audioOutput = nullptr;

SoundPrivate::SoundPrivate()
{
    // NOTE: audioOutput must be initialized in the constructor to avoid static initialization order fiasco
    if (!audioOutput)
        audioOutput = AudioOutput::instance().get();

    player = audioOutput->createAudioPlayer();
}
