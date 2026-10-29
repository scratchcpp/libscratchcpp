// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compilerconstant.h>

#include "cppvalue.h"

namespace libscratchcpp
{

struct CppConstant
    : public CompilerConstant
    , public CppValue
{
        CppConstant(Compiler::StaticType type, const Value &value) :
            CompilerValue(type),
            CompilerConstant(type, value),
            CppValue(type)
        {
            isRawValue = true; // all constants can be read directly
        }

        const Value &constValue() const override final { return CompilerConstant::value(); }
};

} // namespace libscratchcpp
