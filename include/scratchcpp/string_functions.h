// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

#include "global.h"

namespace libscratchcpp
{

struct StringPtr;

extern "C"
{
    LIBSCRATCHCPP_EXPORT void string_alloc(StringPtr *str, size_t size);

    LIBSCRATCHCPP_EXPORT void string_assign(StringPtr *str, const StringPtr *another);
    LIBSCRATCHCPP_EXPORT void string_assign_cstring(StringPtr *str, const char *another);

    LIBSCRATCHCPP_EXPORT int string_compare_case_sensitive(StringPtr *str1, StringPtr *str2);
    LIBSCRATCHCPP_EXPORT int string_compare_case_insensitive(StringPtr *str1, StringPtr *str2);
}

} // namespace libscratchcpp
