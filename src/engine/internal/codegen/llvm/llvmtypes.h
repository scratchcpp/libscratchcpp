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
        static llvm::StructType *createValueDataType(llvm::LLVMContext &ctx);
        static llvm::StructType *createStringPtrType(llvm::LLVMContext &ctx);
        static llvm::Type *createFunctionIdType(llvm::LLVMContext &ctx);
};

} // namespace libscratchcpp
