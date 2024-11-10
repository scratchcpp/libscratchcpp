// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace llvm
{

class Value;
class BasicBlock;

} // namespace llvm

namespace libscratchcpp
{

struct LLVMIfStatement
{
        llvm::Value *condition = nullptr;
        llvm::BasicBlock *beforeIf = nullptr;
        llvm::BasicBlock *body = nullptr;
        llvm::BasicBlock *elseBranch = nullptr;
        llvm::BasicBlock *afterIf = nullptr;
};

} // namespace libscratchcpp
