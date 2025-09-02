// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/istacktimer.h>
#include <chrono>

#include "test_export.h"

namespace libscratchcpp
{

class IClock;

class LIBSCRATCHCPP_TEST_EXPORT StackTimer : public IStackTimer
{
    public:
        StackTimer();
        StackTimer(IClock *clock);
        StackTimer(const StackTimer &) = delete;

        void start(double seconds) override;
        void stop() override;

        bool stopped() const override;
        bool elapsed() const override;
        double elapsedTime() const override;

    private:
        std::chrono::steady_clock::time_point m_startTime;
        bool m_stopped = true;
        long m_timeLimit = 0;
        IClock *m_clock = nullptr;
};

} // namespace libscratchcpp
