// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilervalue.h>
#include <scratchcpp/value.h>

#include <utility>

#include "cppobjectpointer.h"

namespace libscratchcpp
{

class CppInstruction;

struct CppValue : public virtual CompilerValue
{
        CppValue(Compiler::StaticType type) :
            CompilerValue(type)
        {
        }

        virtual const Value &constValue() const
        {
            static const Value null = Value();
            return null;
        }

        bool isRawValue = false;

        std::shared_ptr<CppInstruction> instruction;
};

struct CppPointerValue : public CppValue
{
        CppPointerValue(CppObjectPointerDescriptor descriptor) :
            CompilerValue(Compiler::StaticType::Pointer),
            CppValue(Compiler::StaticType::Pointer),
            descriptor(std::move(descriptor))
        {
            isRawValue = true;
        }

        CppObjectPointerDescriptor descriptor;
};

} // namespace libscratchcpp
