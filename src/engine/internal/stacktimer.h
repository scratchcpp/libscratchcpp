// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/istacktimer.h>
#include <chrono>

namespace libscratchcpp
{

class IClock;

class StackTimer : public IStackTimer
{
    public:
        StackTimer();
        StackTimer(IClock *clock);
        StackTimer(const StackTimer &) = delete;

        void start(double seconds) override;
        void stop() override;

        bool stopped() const override;
        bool elapsed() const override;

    private:
        std::chrono::steady_clock::time_point m_startTime;
        bool m_stopped = true;
        long m_timeLimit = 0;
        IClock *m_clock = nullptr;
};

} // namespace libscratchcpp
