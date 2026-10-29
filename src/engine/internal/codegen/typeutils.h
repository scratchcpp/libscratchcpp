// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <scratchcpp/valuedata.h>

namespace libscratchcpp
{

class TypeUtils
{
    public:
        static Compiler::StaticType mapType(ValueType type);
        static ValueType mapType(Compiler::StaticType type);
        static bool isSingleType(Compiler::StaticType type);
};

} // namespace libscratchcpp
