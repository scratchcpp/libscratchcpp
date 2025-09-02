// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT IAudioEngine
{
    public:
        virtual ~IAudioEngine() { }

        static IAudioEngine *instance();

        virtual float volume() const = 0;
        virtual void setVolume(float volume) = 0;
};

} // namespace libscratchcpp
