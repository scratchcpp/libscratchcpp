// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class IAudioLoudness;

class IAudioInput
{

    public:
        virtual ~IAudioInput() { }

        virtual std::shared_ptr<IAudioLoudness> audioLoudness() const = 0;
};

} // namespace libscratchcpp
