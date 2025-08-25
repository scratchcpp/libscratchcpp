// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilervalue.h>
#include <scratchcpp/value.h>

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

class LLVMInstruction;

struct LLVMRegister : public virtual CompilerValue
{
        LLVMRegister(Compiler::StaticType type) :
            CompilerValue(type)
        {
        }

        virtual const Value &constValue() const
        {
            static const Value null = Value();
            return null;
        }

        llvm::Value *value = nullptr;
        llvm::Value *typeVar = nullptr;

        llvm::Value *isInt = nullptr;
        llvm::Value *intValue = nullptr;

        bool isRawValue = false;

        std::shared_ptr<LLVMInstruction> instruction;
};

} // namespace libscratchcpp
