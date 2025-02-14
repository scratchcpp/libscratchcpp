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

    LIBSCRATCHCPP_EXPORT int string_compare_raw_case_sensitive(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2);
    LIBSCRATCHCPP_EXPORT int string_compare_case_sensitive(const StringPtr *str1, const StringPtr *str2);

    LIBSCRATCHCPP_EXPORT int string_compare_raw_case_insensitive(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2);
    LIBSCRATCHCPP_EXPORT int string_compare_case_insensitive(const StringPtr *str1, const StringPtr *str2);

    LIBSCRATCHCPP_EXPORT bool string_contains_raw_case_sensitive(const char16_t *str, const char16_t *substr);
    LIBSCRATCHCPP_EXPORT bool string_contains_case_sensitive(const StringPtr *str, const StringPtr *substr);

    LIBSCRATCHCPP_EXPORT bool string_contains_raw_case_insensitive(const char16_t *str, const char16_t *substr);
    LIBSCRATCHCPP_EXPORT bool string_contains_case_insensitive(const StringPtr *str, const StringPtr *substr);
}

} // namespace libscratchcpp
