// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value_functions.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/stringptr.h>
#include <algorithm>
#include <limits>
#include <ctgmath>
#include <charconv>
#include <cassert>
#include <utf8.h>

#include "thirdparty/fast_float/fast_float.h"

// Faster than std::isspace()
#define IS_SPACE(x) (x == u' ' || x == u'\f' || x == u'\n' || x == u'\r' || x == u'\t' || x == u'\v')

namespace libscratchcpp
{

static const StringPtr TRUE_STR("true");
static const StringPtr FALSE_STR("false");

template<typename T>
inline unsigned int value_intDigitCount(T v)
{
    unsigned int i = 0;

    while (v >= 1) {
        v /= 10;
        i++;
    }

    return i;
}

template<typename T>
inline unsigned int value_digitCount(T v)
{
    const T epsilon = 0.0000001;
    T intpart;
    unsigned int i = 1, j = 0;

    if (std::abs(std::modf(v, &intpart)) >= epsilon) {
        T tmp_intpart;

        while (std::abs(std::modf(v * std::pow(10, i), &tmp_intpart)) >= epsilon)
            i++;
    }

    while (std::abs(intpart / pow(10, j)) >= 1.0)
        j++;

    return i + j;
}

extern "C"
{
    inline bool value_isInf(double v)
    {
        return v > 0 && std::isinf(v);
    }

    inline bool value_isNegativeInf(double v)
    {
        return v < 0 && std::isinf(v);
    }

    inline double value_convert_int_str(const char16_t *str, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        const char16_t *begin = str;
        const char16_t *end = begin + n;

        bool isNegative = false;
        long ret = 0;

        if (*str == u'-' || *str == u'+') {
            isNegative = (*str == u'-');
            ++str;

            if (str == begin + n)
                return 0;
        }

        while (str < end) {
            if (*str < u'0' || *str > u'9')
                return 0;

            ret = ret * 10 + (*str++ - u'0');
        }

        if (ok)
            *ok = true;

        if (isNegative)
            return -static_cast<double>(ret); // for negative zero
        else
            return ret;
    }

    inline bool value_u16StringsEqual(std::u16string s1, std::u16string s2)
    {
        // TODO: Rewrite this
        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        return (s1.compare(s2) == 0);
    }

    inline bool value_stringsEqual(const char *s1, const char *s2)
    {
        // TODO: Remove this?
        std::string str1(s1);
        std::string str2(s2);
        std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
        std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
        return (str1.compare(str2) == 0);
    }

    inline long value_hexToDec(const char16_t *s, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        // Ignore dots, plus and minus signs
        const char16_t *p = s;

        while (p < s + n) {
            if (*p == u'.' || *p == u'+' || *p == u'-')
                return 0;

            p++;
        }

        long result = 0;
        auto [ptr, ec] = fast_float::from_chars(s, s + n, result, 16);

        if (ec == std::errc{} && ptr == s + n) {
            if (ok)
                *ok = true;

            return result;
        } else
            return 0;
    }

    inline long value_octToDec(const char16_t *s, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        // Ignore plus and minus signs
        const char16_t *p = s;

        while (p < s + n) {
            if (*p == u'+' || *p == u'-')
                return 0;

            p++;
        }

        long result = 0;
        auto [ptr, ec] = fast_float::from_chars(s, s + n, result, 8);

        if (ec == std::errc{} && ptr == s + n) {
            if (ok)
                *ok = true;

            return result;
        } else
            return 0;
    }

    inline double value_binToDec(const char16_t *s, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        // Ignore plus and minus signs
        const char16_t *p = s;

        while (p < s + n) {
            if (*p == u'+' || *p == u'-')
                return 0;

            p++;
        }

        long result = 0;
        auto [ptr, ec] = fast_float::from_chars(s, s + n, result, 2);

        if (ec == std::errc{} && ptr == s + n) {
            if (ok)
                *ok = true;

            return result;
        } else
            return 0;
    }

    inline double value_stringToDoubleImpl(const char16_t *s, size_t n, bool *ok = nullptr)
    {
        if (ok)
            *ok = false;

        if (!s || n == 0)
            return 0;

        const char16_t *begin = s;
        const char16_t *end = s + n;

        // Trim leading spaces
        while (begin < end && IS_SPACE(*begin))
            ++begin;

        // Trim trailing spaces
        while (end > begin && IS_SPACE(*(end - 1)))
            --end;

        if (begin == end) // All spaces case
            return 0;

        // Try to convert integer early
        bool isInt = false;
        double ret = value_convert_int_str(begin, end - begin, &isInt);

        if (isInt) {
            if (ok)
                *ok = true;

            return ret;
        }

        char16_t *copy = nullptr;

        // If there's a leading plus sign, copy the string and replace the plus sign with zero (e. g. '+.15' -> '0.15')
        // If there's a leading minus sign with a dot, insert zero next to the minus sign (e. g. '-.15' -> '-0.15')
        if (begin[0] == u'+' || (begin[0] == u'-' && end - begin > 1 && begin[1] == u'.')) {
            copy = new char16_t[end - begin + 2];

            if (begin[0] == u'-') {
                copy[0] = u'-';
                copy[1] = u'0';
                memcpy(copy + 2, begin + 1, (end - begin - 1) * sizeof(char16_t));
                copy[end - begin + 1] = u'\0';
                end = copy + (end - begin) + 1;
            } else {
                memcpy(copy, begin, (end - begin) * sizeof(char16_t));
                copy[0] = u'0';
                copy[end - begin] = u'\0';
                end = copy + (end - begin);
            }
            begin = copy;
        } else {
            // If there's a leading dot, copy the string and insert zero prior to the dot (e. g. '.15' -> '0.15')
            if (begin[0] == '.') {
                copy = new char16_t[end - begin + 2];
                copy[0] = u'0';
                memcpy(copy + 1, begin, (end - begin) * sizeof(char16_t));
                end = copy + (end - begin) + 1;
                begin = copy;
            }
        }

        if (end - begin <= 0) {
            if (copy)
                delete[] copy;

            return 0;
        }

        // Handle hex, oct and bin (currently ignored if the string was manipulated above)
        if (!copy && end - begin > 2 && begin[0] == u'0') {
            const char16_t prefix = begin[1];
            const char16_t *sub = begin + 2;

            if (prefix == u'x' || prefix == u'X') {
                return value_hexToDec(sub, end - begin - 2, ok);
            } else if (prefix == u'o' || prefix == u'O') {
                return value_octToDec(sub, end - begin - 2, ok);
            } else if (prefix == u'b' || prefix == u'B') {
                return value_binToDec(sub, end - begin - 2, ok);
            }
        }

        // If there's a trailing dot, copy the string and append zero next to it (e. g. '1.' -> '1.0')
        if (end[-1] == u'.') {
            if (copy) {
                char16_t *tmpCopy = new char16_t[end - begin + 1];
                memcpy(tmpCopy, begin, (end - begin) * sizeof(char16_t));
                delete[] copy;
                copy = tmpCopy;
                end = copy + (end - begin) + 1;
                begin = copy;
            } else {
                copy = new char16_t[end - begin + 1];
                memcpy(copy, begin, (end - begin) * sizeof(char16_t));
                end = copy + (end - begin) + 1;
                begin = copy;
            }

            copy[end - begin - 1] = u'0';
        }

        // Trim leading zeros
        bool trimmed = false;

        while (begin < end && (*begin == u'0') && !(begin + 1 < end && begin[1] == u'.')) {
            trimmed = true;
            ++begin;
        }

        if (end - begin <= 0) {
            if (ok)
                *ok = true;

            if (copy)
                delete[] copy;

            return 0;
        }

        // Ignore cases like '0+5' or '0-5'
        if (trimmed && (begin[0] == u'+' || begin[0] == u'-')) {
            if (copy)
                delete[] copy;

            return 0;
        }

        auto [ptr, ec] = fast_float::from_chars(begin, end, ret, fast_float::chars_format::json);

        if (copy)
            delete[] copy;

        if (ec == std::errc{} && ptr == end) {
            if (ok)
                *ok = true;

            return ret;
        }

        // Special values
        // TODO: Use a custom comparison function
        if (value_u16StringsEqual(std::u16string(s), utf8::utf8to16(std::string("Infinity")))) {
            if (ok)
                *ok = true;
            return std::numeric_limits<double>::infinity();
        } else if (value_u16StringsEqual(std::u16string(s), utf8::utf8to16(std::string("-Infinity")))) {
            if (ok)
                *ok = true;
            return -std::numeric_limits<double>::infinity();
        }

        return 0;
    }

    inline long value_stringToLong(const StringPtr *s, bool *ok = nullptr)
    {
        const double ret = value_stringToDoubleImpl(s->data, s->size, ok);

        if (std::isinf(ret))
            return 0;

        return ret;
    }

    inline bool value_stringIsInt(const char16_t *s, int n)
    {
        const char16_t *p = s;

        while (p < s + n) {
            if (*p == u'.' || *p == u'e' || *p == u'E')
                return false;

            p++;
        }

        return true;
    }
}

extern "C"
{
    inline double value_floatToDouble(float v)
    {
        unsigned int digits = value_digitCount(v);
        double f = std::pow(10, digits);
        return std::round(v * f) / f;
    }

    inline int value_checkString(const StringPtr *str)
    {
        bool ok;

        if (value_stringIsInt(str->data, str->size)) {
            value_stringToLong(str, &ok);
            return ok ? 1 : 0;
        } else {
            value_stringToDoubleImpl(str->data, str->size, &ok);
            return ok ? 2 : 0;
        }
    }

    inline double value_getNumber(const ValueData *v, bool *ok = nullptr)
    {
        // Equivalent to JavaScript Number(), *ok == false means NaN
        if (ok)
            *ok = true;

        // Since functions calling this already prioritize int, double and bool,
        // we can optimize by prioritizing the other types here.
        if (v->type == ValueType::String)
            return value_stringToDoubleImpl(v->stringValue->data, v->stringValue->size, ok);
        else if (v->type == ValueType::Number) {
            if (std::isnan(v->numberValue)) {
                *ok = false;
                return 0;
            }

            return v->numberValue;
        } else if (v->type == ValueType::Bool)
            return v->boolValue;
        else {
            assert(false); // this should never happen
            if (ok)
                *ok = false;

            return 0;
        }
    }

    bool value_isPositive(const ValueData *v)
    {
        if (v->type == ValueType::Number)
            return v->numberValue > 0;
        else if (v->type == ValueType::Bool)
            return v->boolValue;

        return value_toDouble(v) > 0;
    }

    bool value_isNegative(const ValueData *v)
    {
        if (v->type == ValueType::Number)
            return v->numberValue < 0;
        else if (v->type == ValueType::Bool)
            return false;

        return value_toDouble(v) < 0;
    }

    bool value_isZero(const ValueData *v)
    {
        if (v->type == ValueType::Number)
            return v->numberValue == 0;
        else if (v->type == ValueType::Bool)
            return !v->boolValue;
        else if (static_cast<int>(v->type) < 0)
            return false;

        bool ok;
        double n = value_getNumber(v, &ok);

        if (ok)
            return n == 0;
        else
            return false;
    }

    inline double value_compare(const ValueData *v1, const ValueData *v2)
    {
        // https://github.com/scratchfoundation/scratch-vm/blob/112989da0e7306eeb405a5c52616e41c2164af24/src/util/cast.js#L121-L150
        assert(v1 && v2);

        if (v1->type == ValueType::Number && v2->type == ValueType::Number && !std::isnan(v1->numberValue) && !std::isnan(v2->numberValue)) {
            // Handle the special case of Infinity
            if ((value_isInf(v1->numberValue) && value_isInf(v2->numberValue)) || (value_isNegativeInf(v1->numberValue) && value_isNegativeInf(v2->numberValue)))
                return 0;

            return v1->numberValue - v2->numberValue;
        } else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool)
            return v1->boolValue - v2->boolValue;

        bool ok;
        double n1 = value_getNumber(v1, &ok);
        double n2;

        if (ok)
            n2 = value_getNumber(v2, &ok);

        if (!ok) {
            // At least one argument can't be converted to a number
            // Scratch compares strings as case insensitive
            // TODO: Use a custom comparison function
            StringPtr *s1 = value_toStringPtr(v1);
            StringPtr *s2 = value_toStringPtr(v2);
            std::string str1 = utf8::utf16to8(std::u16string(s1->data));
            std::string str2 = utf8::utf16to8(std::u16string(s2->data));
            int ret = strcasecmp(str1.c_str(), str2.c_str());
            string_pool_free(s1);
            string_pool_free(s2);
            return ret;
        }

        // Handle the special case of Infinity
        if ((value_isInf(n1) && value_isInf(n2)) || (value_isNegativeInf(n1) && value_isNegativeInf(n2)))
            return 0;

        // Compare as numbers
        return n1 - n2;
    }
}

} // namespace libscratchcpp
