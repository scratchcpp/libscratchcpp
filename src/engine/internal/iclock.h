// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

namespace libscratchcpp
{

class IClock
{
    public:
        virtual ~IClock() { }

        virtual std::chrono::steady_clock::time_point currentSteadyTime() const = 0;
        virtual std::chrono::system_clock::time_point currentSystemTime() const = 0;
};

} // namespace libscratchcpp
