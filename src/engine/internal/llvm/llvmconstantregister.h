// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilerconstant.h>

#include "llvmregister.h"

namespace libscratchcpp
{

struct LLVMConstantRegister
    : public CompilerConstant
    , public LLVMRegister
{
        LLVMConstantRegister(Compiler::StaticType type, const Value &value) :
            CompilerValue(type),
            CompilerConstant(type, value),
            LLVMRegister(type)
        {
        }

        const Value &constValue() const override final { return CompilerConstant::value(); }
};

} // namespace libscratchcpp
