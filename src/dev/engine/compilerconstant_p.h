// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>

namespace libscratchcpp
{

struct CompilerConstantPrivate
{
        CompilerConstantPrivate(const Value &value);

        Value value;
};

} // namespace libscratchcpp
