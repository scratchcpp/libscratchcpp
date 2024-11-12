// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/compiler.h>

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

struct LLVMVariablePtr
{
        llvm::Value *stackPtr = nullptr;
        llvm::Value *heapPtr = nullptr;
        Compiler::StaticType type = Compiler::StaticType::Unknown;
        bool onStack = false;
        bool changed = false;
};

} // namespace libscratchcpp
