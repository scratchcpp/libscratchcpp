// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "iclock.h"
#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT Clock : public IClock
{
    public:
        Clock();
        Clock(const Clock &) = delete;

        static std::shared_ptr<Clock> instance();

        std::chrono::steady_clock::time_point currentSteadyTime() const override;
        std::chrono::system_clock::time_point currentSystemTime() const override;

        void sleep(const std::chrono::milliseconds &time) const override;

    private:
        static std::shared_ptr<Clock> m_instance;
};

} // namespace libscratchcpp
