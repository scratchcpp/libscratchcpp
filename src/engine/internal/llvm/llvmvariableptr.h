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

struct LLVMVariablePtr
{
        llvm::Value *stackPtr = nullptr;
        llvm::Value *heapPtr = nullptr;
        Compiler::StaticType type = Compiler::StaticType::Unknown;
        bool onStack = false;
        bool changed = false;

        // Used in build phase to check the type safety of variables in loops
        std::unordered_map<LLVMLoopScope *, std::vector<LLVMInstruction *>> loopVariableWrites; // loop scope, write instructions
};

} // namespace libscratchcpp
