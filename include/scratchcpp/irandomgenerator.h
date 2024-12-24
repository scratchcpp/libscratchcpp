// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

/*! \brief The IRandomGenerator interface represents a random number generator that can be received e. g. from an ExecutionContext. */
class LIBSCRATCHCPP_EXPORT IRandomGenerator
{
    public:
        virtual ~IRandomGenerator() { }

        /*! Returns a random integer in the given range (inclusive). */
        virtual long randint(long start, long end) const = 0;

        /*! Returns a random double in the given range (inclusive). */
        virtual double randintDouble(double start, double end) const = 0;

        /*! Returns a random integer in the given range (inclusive) except the given integer. */
        virtual long randintExcept(long start, long end, long except) const = 0;
};

} // namespace libscratchcpp
