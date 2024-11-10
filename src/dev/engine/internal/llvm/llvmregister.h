// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/value.h>

namespace llvm
{

class Value;

}

namespace libscratchcpp
{

struct LLVMRegister
{
        LLVMRegister(Compiler::StaticType type) :
            type(type)
        {
        }

        Compiler::StaticType type = Compiler::StaticType::Void;
        llvm::Value *value = nullptr;
        bool isRawValue = false;
        bool isConstValue = false;
        Value constValue;
};

using LLVMRegisterPtr = std::shared_ptr<LLVMRegister>;

} // namespace libscratchcpp
