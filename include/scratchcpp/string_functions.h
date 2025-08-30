// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "stringptr.h"

namespace libscratchcpp
{

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

static inline bool raw_strings_equal_case_sensitive(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2)
{
    return n1 == n2 && string_compare_raw_case_sensitive(str1, n1, str2, n2) == 0;
}

static inline bool strings_equal_case_sensitive(const StringPtr *str1, const StringPtr *str2)
{
    return raw_strings_equal_case_sensitive(str1->data, str1->size, str2->data, str2->size);
}

static inline bool raw_strings_equal_case_insensitive(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2)
{
    return n1 == n2 && string_compare_raw_case_insensitive(str1, n1, str2, n2) == 0;
}

static inline bool strings_equal_case_insensitive(const StringPtr *str1, const StringPtr *str2)
{
    return raw_strings_equal_case_insensitive(str1->data, str1->size, str2->data, str2->size);
}

} // namespace libscratchcpp
