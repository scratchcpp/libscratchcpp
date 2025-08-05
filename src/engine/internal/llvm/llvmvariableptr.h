// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

class LLVMInstruction;

struct LLVMVariablePtr
{
        llvm::Value *stackPtr = nullptr;
        llvm::Value *heapPtr = nullptr;
        bool onStack = false;
        bool changed = false;
};

} // namespace libscratchcpp
