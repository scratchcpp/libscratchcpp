// SPDX-License-Identifier: Apache-2.0

#include <cassert>

#include "timer.h"
#include "clock.h"

using namespace libscratchcpp;

Timer::Timer()
{
    m_clock = Clock::instance().get();
    resetTimer();
}

Timer::Timer(IClock *clock) :
    m_clock(clock)
{
    assert(clock);
    resetTimer();
}

double Timer::value() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_clock->currentTime() - m_startTime).count() / 1000.0;
}

void Timer::reset()
{
    resetTimer();
}

// Required to avoid calling the virtual method from the constructors
void Timer::resetTimer()
{
    m_startTime = m_clock->currentTime();
}
