// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../iaudioloudness.h"
#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT AudioLoudnessStub : public IAudioLoudness
{
    public:
        AudioLoudnessStub();

        int getLoudness() const override;
};

} // namespace libscratchcpp
