// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iaudiooutput.h"
#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT AudioOutput : public IAudioOutput
{
    public:
        AudioOutput();

        static std::shared_ptr<IAudioOutput> instance();

        std::shared_ptr<IAudioPlayer> createAudioPlayer() const override;
};

} // namespace libscratchcpp
