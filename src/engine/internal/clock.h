// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include "iclock.h"

namespace libscratchcpp
{

class Clock : public IClock
{
    public:
        Clock();
        Clock(const Clock &) = delete;

        static std::shared_ptr<Clock> instance();

        std::chrono::steady_clock::time_point currentTime() const override;

    private:
        static std::shared_ptr<Clock> m_instance;
};

} // namespace libscratchcpp
