// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/compiler.h>

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

struct LLVMListPtr
{
        llvm::Value *ptr = nullptr;
        llvm::Value *dataPtr = nullptr;
        llvm::Value *sizePtr = nullptr;
        Compiler::StaticType type = Compiler::StaticType::Unknown;
};

} // namespace libscratchcpp
