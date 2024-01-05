// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iaudioplayerfactory.h"

namespace libscratchcpp
{

class AudioPlayerFactory : public IAudioPlayerFactory
{
    public:
        AudioPlayerFactory();

        static std::shared_ptr<AudioPlayerFactory> instance();
        std::shared_ptr<IAudioPlayer> create() const override;

    private:
        static std::shared_ptr<AudioPlayerFactory> m_instance;
};

} // namespace libscratchcpp
