// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace llvm
{

class Value;
class BasicBlock;

} // namespace llvm

namespace libscratchcpp
{

struct LLVMLoop
{
        bool isRepeatLoop = false;
        llvm::Value *index = nullptr;
        llvm::BasicBlock *conditionBranch = nullptr;
        llvm::BasicBlock *afterLoop = nullptr;
};

} // namespace libscratchcpp
