// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

#include "../instruction.h"
#include "cppvalue.h"

namespace libscratchcpp
{

struct CppInstruction : Instruction<CppInstruction, CppValue>
{
        using Type = Instruction<CppInstruction, CppValue>::Type;
        using Instruction<CppInstruction, CppValue>::Instruction; // inherit constructor
};

} // namespace libscratchcpp
