// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>
#include <cmath>
#include <cstring>
#include <cassert>
#include <utf8.h>

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
}

} // namespace libscratchcpp
