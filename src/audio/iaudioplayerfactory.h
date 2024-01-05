// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class IAudioPlayer;

class IAudioPlayerFactory
{
    public:
        virtual ~IAudioPlayerFactory() { }

        virtual std::shared_ptr<IAudioPlayer> create() const = 0;
};

} // namespace libscratchcpp
