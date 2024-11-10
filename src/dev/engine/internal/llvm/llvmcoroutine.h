// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace llvm
{

class Value;
class BasicBlock;

} // namespace llvm

namespace libscratchcpp
{

struct LLVMCoroutine
{
        llvm::Value *handle = nullptr;
        llvm::BasicBlock *suspend = nullptr;
        llvm::BasicBlock *cleanup = nullptr;
        llvm::BasicBlock *freeMemRet = nullptr;
        llvm::Value *didSuspend = nullptr;
};

} // namespace libscratchcpp
