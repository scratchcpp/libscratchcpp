// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <llvm/IR/IRBuilder.h>

namespace llvm
{

class StructValue;

}

namespace libscratchcpp
{

class LLVMTypes
{
    public:
        static llvm::StructType *createValueDataType(llvm::IRBuilder<> *builder);
        static llvm::StructType *createStringPtrType(llvm::IRBuilder<> *builder);
};

} // namespace libscratchcpp
