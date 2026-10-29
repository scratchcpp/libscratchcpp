// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilervalue.h>
#include <scratchcpp/value.h>

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

} // namespace libscratchcpp
