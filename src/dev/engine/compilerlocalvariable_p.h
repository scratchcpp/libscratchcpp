// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class CompilerValue;

struct CompilerLocalVariablePrivate
{
        CompilerLocalVariablePrivate(CompilerValue *ptr);
        CompilerLocalVariablePrivate(CompilerLocalVariablePrivate &) = delete;

        CompilerValue *ptr = nullptr;
};

} // namespace libscratchcpp
