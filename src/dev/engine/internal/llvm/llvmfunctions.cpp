// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/value_functions.h>

#include "llvmfunctions.h"
#include "../../../../engine/internal/randomgenerator.h"

namespace libscratchcpp
{

extern "C"
{
    double llvm_random(ValueData *from, ValueData *to)
    {
        if (!llvm_rng)
            llvm_rng = RandomGenerator::instance().get();

        return value_isInt(from) && value_isInt(to) ? llvm_rng->randint(value_toLong(from), value_toLong(to)) : llvm_rng->randintDouble(value_toDouble(from), value_toDouble(to));
    }

    double llvm_random_double(double from, double to)
    {
        if (!llvm_rng)
            llvm_rng = RandomGenerator::instance().get();

        return value_doubleIsInt(from) && value_doubleIsInt(to) ? llvm_rng->randint(from, to) : llvm_rng->randintDouble(from, to);
    }

    double llvm_random_bool(bool from, bool to)
    {
        if (!llvm_rng)
            llvm_rng = RandomGenerator::instance().get();

        return llvm_rng->randint(from, to);
    }
}

} // namespace libscratchcpp
