// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>
#include <cmath>
#include <cstring>
#include <cassert>
#include <utf8.h>
#include <unicodelib.h>
#include <unicodelib_encodings.h>

namespace libscratchcpp
{

extern "C"
{

    /*! Ensures at least the given size is allocated on str. */
    void string_alloc(StringPtr *str, size_t size)
    {
        size++; // null terminator

        if (str->allocatedSize < size) {
            // TODO: Use std::bit_ceil() in C++20
            const size_t newSize = std::pow(2, std::ceil(std::log2(size)));
            assert((str->data && str->allocatedSize > 0) || (!str->data && str->allocatedSize == 0));

            if (str->data)
                str->data = (typeof(str->data))realloc(str->data, newSize * sizeof(typeof(*str->data)));
            else
                str->data = (typeof(str->data))malloc(newSize * sizeof(typeof(*str->data)));

            str->allocatedSize = newSize;
        }
    }

    /*! Assigns the given string to str. */
    void string_assign(StringPtr *str, const StringPtr *another)
    {
        string_alloc(str, another->size);
        str->size = another->size;
        memcpy(str->data, another->data, (another->size + 1) * sizeof(typeof(*str->data)));
    }

    /*! Assigns the given string to str. */
    void string_assign_cstring(StringPtr *str, const char *another)
    {
        if (!another) {
            str->size = 0;
            string_alloc(str, 0);
            str->data[0] = u'\0';
            return;
        }

        // TODO: Use a library to map characters
        std::u16string converted = utf8::utf8to16(std::string(another));
        string_alloc(str, converted.size());
        str->size = converted.size();
        memcpy(str->data, converted.data(), (converted.size() + 1) * sizeof(typeof(*str->data)));
    }

    inline int string_compare_raw_case_sensitive_inline(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2)
    {
        const size_t min_len = std::min(n1, n2);

        for (size_t i = 0; i < min_len; i++) {
            if (str1[i] != str2[i])
                return str1[i] - str2[i];
        }

        return n1 < n2 ? -1 : (n1 > n2 ? 1 : 0);
    }

    int string_compare_raw_case_sensitive(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2)
    {
        return string_compare_raw_case_sensitive_inline(str1, n1, str2, n2);
    }

    int string_compare_case_sensitive(const StringPtr *str1, const StringPtr *str2)
    {
        return string_compare_raw_case_sensitive_inline(str1->data, str1->size, str2->data, str2->size);
    }

    inline int string_compare_raw_case_insensitive_inline(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2)
    {
        const size_t min_len = std::min(n1, n2);
        char32_t cp1, cp2;

        for (size_t i = 0; i < min_len; ++i) {
            std::u32string cp1_str, cp2_str;
            unicode::utf16::decode(str1 + i, 1, cp1_str);
            unicode::utf16::decode(str2 + i, 1, cp2_str);

            cp1 = unicode::simple_lowercase_mapping(cp1_str.front());
            cp2 = unicode::simple_lowercase_mapping(cp2_str.front());

            if (cp1 != cp2)
                return cp1 - cp2;
        }

        return n1 < n2 ? -1 : (n1 > n2 ? 1 : 0);
    }

    int string_compare_raw_case_insensitive(const char16_t *str1, size_t n1, const char16_t *str2, size_t n2)
    {
        return string_compare_raw_case_insensitive_inline(str1, n1, str2, n2);
    }

    int string_compare_case_insensitive(const StringPtr *str1, const StringPtr *str2)
    {
        return string_compare_raw_case_insensitive_inline(str1->data, str1->size, str2->data, str2->size);
    }
}

} // namespace libscratchcpp
