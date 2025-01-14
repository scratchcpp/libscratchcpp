// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

struct CompilerValuePrivate
{
        CompilerValuePrivate(Compiler::StaticType type);

        Compiler::StaticType type = Compiler::StaticType::Unknown;
};

} // namespace libscratchcpp
