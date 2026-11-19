// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

struct CppObjectPointerDescriptor
{
        enum class Kind
        {
            Target,
            Variable,
            List,
            Timer,
            Effect
        };

        CppObjectPointerDescriptor(Kind kind, const std::string &key = "") :
            kind(kind),
            key(key)
        {
        }

        Kind kind;
        std::string key;
};

} // namespace libscratchcpp
