// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilerconstant.h>

#include "llvmregister.h"

namespace libscratchcpp
{

struct LLVMConstantRegister
    : public LLVMRegisterBase
    , public CompilerConstant
{
        LLVMConstantRegister(Compiler::StaticType type, const Value &value) :
            LLVMRegisterBase(),
            CompilerConstant(type, value)
        {
        }

        const Value &constValue() const override { return CompilerConstant::value(); }
};

} // namespace libscratchcpp
