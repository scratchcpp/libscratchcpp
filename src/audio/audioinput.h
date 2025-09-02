// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iaudioinput.h"
#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT AudioInput : public IAudioInput
{
    public:
        AudioInput();

        static std::shared_ptr<IAudioInput> instance();

        std::shared_ptr<IAudioLoudness> audioLoudness() const override;

    private:
        mutable std::shared_ptr<IAudioLoudness> m_audioLoudness;
};

} // namespace libscratchcpp
