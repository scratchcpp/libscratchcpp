// SPDX-License-Identifier: Apache-2.0

#include "clock.h"

using namespace libscratchcpp;

std::shared_ptr<Clock> Clock::m_instance = std::make_shared<Clock>();

Clock::Clock()
{
}

std::shared_ptr<Clock> Clock::instance()
{
    return m_instance;
}

std::chrono::steady_clock::time_point Clock::currentTime() const
{
    return std::chrono::steady_clock::now();
}
