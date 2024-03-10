// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioloudness.h"

struct ma_device;

namespace libscratchcpp
{

class AudioLoudness : public IAudioLoudness
{
    public:
        AudioLoudness();
        ~AudioLoudness();

        int getLoudness() const override;

    private:
        ma_device *m_device = nullptr;
};

} // namespace libscratchcpp
