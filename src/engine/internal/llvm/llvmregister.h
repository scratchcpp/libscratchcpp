// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "llvmregisterbase.h"

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

struct LLVMRegister
    : public LLVMRegisterBase
    , public CompilerValue
{
        LLVMRegister(Compiler::StaticType type) :
            LLVMRegisterBase(),
            CompilerValue(type)
        {
        }

        const Value &constValue() const override
        {
            if (isConst())
                return static_cast<const CompilerConstant *>(static_cast<const CompilerValue *>(this))->value();
            else {
                static const Value null = Value();
                return null;
            }
        }
};

} // namespace libscratchcpp
