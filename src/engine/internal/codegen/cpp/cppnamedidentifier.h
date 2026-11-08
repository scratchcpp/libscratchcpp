// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>

#include "cppidentifier.h"

namespace libscratchcpp
{

struct CppNamedIdentifier : public CppIdentifier
{
        CppNamedIdentifier() { }

        CppNamedIdentifier(Type type, const std::string &name, bool isMutable = true) :
            CppIdentifier(type),
            name(name),
            isMutable(isMutable)
        {
        }

        virtual bool isNamed() const override final { return true; }

        std::string name;
        bool isMutable = true;
};

} // namespace libscratchcpp
