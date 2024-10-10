// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class IRandomGenerator
{
    public:
        virtual ~IRandomGenerator() { }

        virtual long randint(long start, long end) const = 0;
        virtual double randintDouble(double start, double end) const = 0;
        virtual long randintExcept(long start, long end, long except) const = 0;
};

} // namespace libscratchcpp
