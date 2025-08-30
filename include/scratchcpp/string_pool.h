// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

struct StringPtr;

extern "C"
{
    LIBSCRATCHCPP_EXPORT StringPtr *string_pool_new();
    LIBSCRATCHCPP_EXPORT void string_pool_free(StringPtr *str);
}

} // namespace libscratchcpp
