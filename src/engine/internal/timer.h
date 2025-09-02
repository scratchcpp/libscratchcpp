// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/itimer.h>
#include <chrono>

#include "test_export.h"

namespace libscratchcpp
{

class IClock;

class LIBSCRATCHCPP_TEST_EXPORT Timer : public ITimer
{
    public:
        Timer();
        Timer(IClock *clock);
        Timer(const Timer &) = delete;

        double value() const override;
        void reset() override;

    private:
        void resetTimer();

        std::chrono::steady_clock::time_point m_startTime;
        IClock *m_clock = nullptr;
};

} // namespace libscratchcpp
