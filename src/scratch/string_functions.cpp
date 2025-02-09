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

    int string_compare_case_sensitive(StringPtr *str1, StringPtr *str2)
    {
        if (str1->size != str2->size)
            return str1->size < str2->size ? -1 : 1;

        const size_t min_len = std::min(str1->size, str2->size);

        for (size_t i = 0; i < min_len; i++) {
            if (str1->data[i] != str2->data[i])
                return str1->data[i] - str2->data[i];
        }

        return 0;
    }

    int string_compare_case_insensitive(StringPtr *str1, StringPtr *str2)
    {
        if (str1->size != str2->size)
            return str1->size < str2->size ? -1 : 1;

        const size_t min_len = std::min(str1->size, str2->size);
        std::u32string cp1_str, cp2_str;
        char32_t cp1, cp2;

        for (size_t i = 0; i < min_len; ++i) {
            unicode::utf16::decode(str1->data + i, 1, cp1_str);
            unicode::utf16::decode(str2->data + i, 1, cp2_str);

            cp1 = unicode::simple_lowercase_mapping(cp1_str.front());
            cp2 = unicode::simple_lowercase_mapping(cp2_str.front());

            if (cp1 != cp2)
                return cp1 - cp2;
        }

        return 0;
    }
}

} // namespace libscratchcpp
