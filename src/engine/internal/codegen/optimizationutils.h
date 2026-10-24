// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

class OptimizationUtils
{
    public:
        static Compiler::StaticType optimizeValueType(CompilerContext *ctx, const CompilerValue *value);
};

} // namespace libscratchcpp
