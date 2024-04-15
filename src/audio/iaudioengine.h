// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class IAudioEngine
{
    public:
        virtual ~IAudioEngine() { }

        static IAudioEngine *instance();

        virtual float volume() const = 0;
        virtual void setVolume(float volume) = 0;
};

} // namespace libscratchcpp
