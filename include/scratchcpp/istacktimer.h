// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

/*!
 * \brief The IStackTimer interface represents a timer that can be used by blocks.
 *
 * You can get a stack timer using ExecutionContext#stackTimer().
 */
class LIBSCRATCHCPP_EXPORT IStackTimer
{
    public:
        virtual ~IStackTimer() { }

        /*! Starts the timer. */
        virtual void start(double seconds) = 0;

        /*! Stops the timer. */
        virtual void stop() = 0;

        /*! Returns true if the timer has been stopped using stop() or wasn't used at all. */
        virtual bool stopped() const = 0;

        /*! Returns true if the timer has elapsed. */
        virtual bool elapsed() const = 0;
};

} // namespace libscratchcpp
