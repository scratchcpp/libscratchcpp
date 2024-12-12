// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <iostream>

#include "stacktimer.h"
#include "clock.h"

using namespace libscratchcpp;

StackTimer::StackTimer()
{
    m_clock = Clock::instance().get();
}

StackTimer::StackTimer(IClock *clock) :
    m_clock(clock)
{
    assert(clock);
}

void StackTimer::start(double seconds)
{
    m_startTime = m_clock->currentSteadyTime();
    m_timeLimit = seconds * 1000;
    m_stopped = false;
}

void StackTimer::stop()
{
    m_stopped = true;
}

bool StackTimer::stopped() const
{
    return m_stopped;
}

bool StackTimer::elapsed() const
{
    if (m_stopped)
        return false;

    return std::chrono::duration_cast<std::chrono::milliseconds>(m_clock->currentSteadyTime() - m_startTime).count() >= m_timeLimit;
}
