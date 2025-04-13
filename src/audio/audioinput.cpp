// SPDX-License-Identifier: Apache-2.0

#include "audioinput.h"

#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
#include "internal/audioloudness.h"
#else
#include "internal/audioloudnessstub.h"
#endif

using namespace libscratchcpp;

static std::shared_ptr<IAudioInput> INSTANCE = std::make_shared<AudioInput>();

AudioInput::AudioInput()
{
}

std::shared_ptr<IAudioInput> AudioInput::instance()
{
    return INSTANCE;
}

std::shared_ptr<IAudioLoudness> AudioInput::audioLoudness() const
{
    if (!m_audioLoudness) {
#ifdef LIBSCRATCHCPP_AUDIO_SUPPORT
        m_audioLoudness = std::make_shared<AudioLoudness>();
#else
        m_audioLoudness = std::make_shared<AudioLoudnessStub>();
#endif
    }

    return m_audioLoudness;
}
