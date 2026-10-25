// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

struct LLVMLocalVariableInfo
{
        llvm::Value *isInt = nullptr;
        llvm::Value *intValue = nullptr;
};

} // namespace libscratchcpp
