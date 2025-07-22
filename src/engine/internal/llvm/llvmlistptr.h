// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <unordered_map>

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

class LLVMLoopScope;
class LLVMInstruction;

struct LLVMListPtr
{
        llvm::Value *ptr = nullptr;
        llvm::Value *dataPtr = nullptr;
        llvm::Value *sizePtr = nullptr;
        llvm::Value *allocatedSizePtr = nullptr;
        llvm::Value *dataPtrDirty = nullptr;
        Compiler::StaticType type = Compiler::StaticType::Unknown;

        // Used in build phase to check the type safety of lists in loops
        std::unordered_map<LLVMLoopScope *, std::vector<LLVMInstruction *>> loopListWrites; // loop scope, write instructions
};

} // namespace libscratchcpp
