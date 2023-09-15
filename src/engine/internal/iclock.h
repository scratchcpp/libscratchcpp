// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

namespace libscratchcpp
{

class IClock
{
    public:
        virtual ~IClock() { }

        virtual std::chrono::steady_clock::time_point currentTime() const = 0;
};

} // namespace libscratchcpp
