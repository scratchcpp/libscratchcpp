// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iaudiooutput.h"

namespace libscratchcpp
{

class AudioOutput : public IAudioOutput
{
    public:
        AudioOutput();

        static std::shared_ptr<IAudioOutput> instance();

        std::shared_ptr<IAudioPlayer> createAudioPlayer() const override;

    private:
        static std::shared_ptr<IAudioOutput> m_instance;
};

} // namespace libscratchcpp
