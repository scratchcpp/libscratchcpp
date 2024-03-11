// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "iaudioinput.h"

namespace libscratchcpp
{

class AudioInput : public IAudioInput
{
    public:
        AudioInput();

        static std::shared_ptr<IAudioInput> instance();

        std::shared_ptr<IAudioLoudness> audioLoudness() const override;

    private:
        static std::shared_ptr<IAudioInput> m_instance;

        mutable std::shared_ptr<IAudioLoudness> m_audioLoudness;
};

} // namespace libscratchcpp
