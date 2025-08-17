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

struct LLVMListPtr
{
        llvm::Value *ptr = nullptr;
        llvm::Value *dataPtr = nullptr;
        llvm::Value *sizePtr = nullptr;
        llvm::Value *allocatedSizePtr = nullptr;
        llvm::Value *size = nullptr;
        llvm::Value *type = nullptr;
};

} // namespace libscratchcpp
