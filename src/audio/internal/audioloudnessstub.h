// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioloudness.h"

namespace libscratchcpp
{

class AudioLoudnessStub : public IAudioLoudness
{
    public:
        AudioLoudnessStub();

        int getLoudness() const override;
};

} // namespace libscratchcpp
