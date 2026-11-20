// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>

#include "cppidentifier.h"

namespace libscratchcpp
{

struct CppConstIdentifier : public CppIdentifier
{
        CppConstIdentifier() { }

        CppConstIdentifier(Type type, const Value &value) :
            CppIdentifier(type),
            value(value)
        {
        }

        virtual bool isConst() const override final { return true; }

        Value value;
};

} // namespace libscratchcpp
