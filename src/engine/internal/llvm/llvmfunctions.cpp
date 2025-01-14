// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/value_functions.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/irandomgenerator.h>

namespace libscratchcpp
{

extern "C"
{
    double llvm_random(ExecutionContext *ctx, ValueData *from, ValueData *to)
    {
        return value_isInt(from) && value_isInt(to) ? ctx->rng()->randint(value_toLong(from), value_toLong(to)) : ctx->rng()->randintDouble(value_toDouble(from), value_toDouble(to));
    }

    double llvm_random_double(ExecutionContext *ctx, double from, double to)
    {
        return value_doubleIsInt(from) && value_doubleIsInt(to) ? ctx->rng()->randint(from, to) : ctx->rng()->randintDouble(from, to);
    }

    double llvm_random_long(ExecutionContext *ctx, long from, long to)
    {
        return ctx->rng()->randint(from, to);
    }

    double llvm_random_bool(ExecutionContext *ctx, bool from, bool to)
    {
        return ctx->rng()->randint(from, to);
    }
}

} // namespace libscratchcpp
