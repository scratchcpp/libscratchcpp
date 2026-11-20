// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

struct CppTypeInfo
{
        const std::string name;
        const bool isMemberAccessible = false;
        const bool isPointer = false;
        const bool isArray = false;
};

} // namespace libscratchcpp
