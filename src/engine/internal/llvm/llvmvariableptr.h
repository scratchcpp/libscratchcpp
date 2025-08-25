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
        llvm::Value *heapPtr = nullptr;
        llvm::Value *stackPtr = nullptr;

        llvm::Value *isInt = nullptr;
        llvm::Value *intValue = nullptr;
};

} // namespace libscratchcpp
