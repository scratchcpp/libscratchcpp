// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class IAudioPlayer;

class IAudioOutput
{
    public:
        virtual ~IAudioOutput() { }

        virtual std::shared_ptr<IAudioPlayer> createAudioPlayer() const = 0;
};

} // namespace libscratchcpp
