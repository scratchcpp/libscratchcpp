// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

/*!
 * \brief The ITimer interface represents a timer of a Scratch project.
 *
 * You can get a project timer using Engine#timer().
 */
class ITimer
{
    public:
        virtual ~ITimer() { }

        /*! Returns the time since last reset in seconds. */
        virtual double value() const = 0;

        /*! Resets the timer. */
        virtual void reset() = 0;
};

} // namespace libscratchcpp
