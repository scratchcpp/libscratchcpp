// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/value.h>

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

struct LLVMRegisterBase
{
        virtual const Value &constValue() const = 0;

        llvm::Value *value = nullptr;
        bool isRawValue = false;
};

} // namespace libscratchcpp
