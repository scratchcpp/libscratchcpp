// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

#include "../instruction.h"
#include "llvmregister.h"

namespace libscratchcpp
{

class LLVMLocalVariableInfo;
class BlockPrototype;

struct LLVMInstruction : Instruction<LLVMInstruction, LLVMRegister>
{
        using Type = Instruction<LLVMInstruction, LLVMRegister>::Type;
        using Instruction<LLVMInstruction, LLVMRegister>::Instruction; // inherit constructor

        LLVMLocalVariableInfo *localVarInfo = nullptr; // for local variables
};

} // namespace libscratchcpp
