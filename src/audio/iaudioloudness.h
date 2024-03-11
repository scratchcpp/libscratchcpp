// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class IAudioLoudness
{
    public:
        virtual ~IAudioLoudness() { }

        virtual int getLoudness() const = 0;
};

} // namespace libscratchcpp
