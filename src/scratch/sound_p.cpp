// SPDX-License-Identifier: Apache-2.0

#include "sound_p.h"
#include "audio/audiooutput.h"

using namespace libscratchcpp;

SoundPrivate::SoundPrivate()
{
    // NOTE: audioOutput must be initialized in the constructor to avoid static initialization order fiasco
    if (!m_audioOutput)
        m_audioOutput = AudioOutput::instance().get();

    player = m_audioOutput->createAudioPlayer();
}

void SoundPrivate::setAudioOutput(IAudioOutput *audioOutput)
{
    m_audioOutput = audioOutput;
}
