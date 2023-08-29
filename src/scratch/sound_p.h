// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

struct SoundPrivate
{
        SoundPrivate();
        SoundPrivate(const SoundPrivate &) = delete;

        int rate = 0;
        int sampleCount = 0;
};

} // namespace libscratchcpp
