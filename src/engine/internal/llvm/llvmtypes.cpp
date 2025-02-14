// SPDX-License-Identifier: Apache-2.0

#include <llvm/IR/IRBuilder.h>

#include "llvmtypes.h"

using namespace libscratchcpp;

llvm::StructType *LLVMTypes::createValueDataType(llvm::IRBuilder<> *builder)
{
    llvm::LLVMContext &ctx = builder->getContext();

    // Create the ValueData struct
    llvm::Type *unionType = builder->getInt64Ty(); // 64 bits is the largest size

    llvm::Type *valueType = llvm::Type::getInt32Ty(ctx); // Assuming ValueType is a 32-bit enum
    llvm::Type *padding = llvm::Type::getInt32Ty(ctx);   // Padding for alignment

    llvm::StructType *ret = llvm::StructType::create(ctx, "ValueData");
    ret->setBody({ unionType, valueType, padding });

    return ret;
}

llvm::StructType *LLVMTypes::createStringPtrType(llvm::IRBuilder<> *builder)
{
    llvm::LLVMContext &ctx = builder->getContext();

    // Create the StringPtr struct
    llvm::PointerType *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    llvm::Type *sizeType = llvm::Type::getInt64Ty(ctx);
    llvm::StructType *ret = llvm::StructType::create(ctx, "StringPtr");
    ret->setBody({ pointerType, sizeType, sizeType });

    return ret;
}
