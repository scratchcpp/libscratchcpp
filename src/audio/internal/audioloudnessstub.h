// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioloudness.h"

namespace libscratchcpp
{

class AudioLoudness : public IAudioLoudness
{
    public:
        AudioLoudness();

        int getLoudness() const override;
};

} // namespace libscratchcpp
