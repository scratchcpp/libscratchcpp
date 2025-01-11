// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value_functions.h>
#include <algorithm>
#include <limits>
#include <ctgmath>
#include <iomanip>
#include <charconv>
#include <cassert>

#include "thirdparty/fast_float/fast_float.h"

// Faster than std::isspace()
#define IS_SPACE(x) (x == ' ' || x == '\f' || x == '\n' || x == '\r' || x == '\t' || x == '\v')

namespace libscratchcpp
{

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

    inline double value_convert_int_str(const char *str, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        const char *begin = str;
        const char *end = begin + n;

        bool isNegative = false;
        long ret = 0;

        if (*str == '-' || *str == '+') {
            isNegative = (*str == '-');
            ++str;

            if (str == begin + n)
                return 0;
        }

        while (str < end) {
            if (*str < '0' || *str > '9')
                return 0;

            ret = ret * 10 + (*str++ - '0');
        }

        if (ok)
            *ok = true;

        if (isNegative)
            return -static_cast<double>(ret); // for negative zero
        else
            return ret;
    }

    inline size_t value_getSize(size_t x)
    {
        if (x == 0)
            return 0;

        size_t ret = 1;

        while (ret < x)
            ret *= 2;

        return ret;
    }

    inline void value_initStr(ValueData *v, const char *s)
    {
        const size_t len = strlen(s);
        v->stringSize = value_getSize(len + 1);
        v->type = ValueType::String;
        v->stringValue = (char *)malloc((v->stringSize * sizeof(char)));
        memcpy(v->stringValue, s, len);
        v->stringValue[len] = '\0';
    }

    inline void value_replaceStr(ValueData *v, const char *s)
    {
        const size_t len = strlen(s);
        const size_t size = value_getSize(len + 1);

        if (size == 0)
            return;

        if (size > v->stringSize || v->stringSize / size > 3) {
            v->stringSize = size;
            v->stringValue = (char *)realloc(v->stringValue, v->stringSize * sizeof(char));
        }

        memcpy(v->stringValue, s, len);
        v->stringValue[len] = '\0';
    }

    inline bool value_u16StringsEqual(std::u16string s1, std::u16string s2)
    {
        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        return (s1.compare(s2) == 0);
    }

    inline bool value_stringsEqual(const char *s1, const char *s2)
    {
        std::string str1(s1);
        std::string str2(s2);
        std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
        std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
        return (str1.compare(str2) == 0);
    }

    inline long value_hexToDec(const char *s, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        // Ignore dots, plus and minus signs
        const char *p = s;

        while (p < s + n) {
            if (*p == '.' || *p == '+' || *p == '-')
                return 0;

            p++;
        }

        long result = 0;
        auto [ptr, ec] = std::from_chars(s, s + n, result, 16);

        if (ec == std::errc{} && ptr == s + n) {
            if (ok)
                *ok = true;

            return result;
        } else
            return 0;
    }

    inline long value_octToDec(const char *s, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        // Ignore plus and minus signs
        const char *p = s;

        while (p < s + n) {
            if (*p == '+' || *p == '-')
                return 0;

            p++;
        }

        char *err;
        const double ret = std::strtol(s, &err, 8);

        if (*err != 0 && !std::isspace(*err))
            return 0;
        else {
            if (ok)
                *ok = true;

            return ret;
        }
    }

    inline double value_binToDec(const char *s, int n, bool *ok)
    {
        if (ok)
            *ok = false;

        // Ignore plus and minus signs
        const char *p = s;

        while (p < s + n) {
            if (*p == '+' || *p == '-')
                return 0;

            p++;
        }

        char *err;
        const double ret = std::strtol(s, &err, 2);

        if (*err != 0 && !std::isspace(*err))
            return 0;
        else {
            if (ok)
                *ok = true;

            return ret;
        }
    }

    inline double value_stringToDoubleImpl(const char *s, bool *ok = nullptr)
    {
        if (ok)
            *ok = false;

        if (!s || *s == '\0')
            return 0;

        const size_t len = strlen(s);
        const char *begin = s;
        const char *end = s + len;

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

        char *copy = nullptr;

        // If there's a leading plus sign, copy the string and replace the plus sign with zero (e. g. '+.15' -> '0.15')
        // If there's a leading minus sign with a dot, insert zero next to the minus sign (e. g. '-.15' -> '-0.15')
        if (begin[0] == '+' || (begin[0] == '-' && end - begin > 1 && begin[1] == '.')) {
            copy = new char[end - begin + 2];

            if (begin[0] == '-') {
                copy[0] = '-';
                copy[1] = '0';
                memcpy(copy + 2, begin + 1, end - begin - 1);
                copy[end - begin + 1] = '\0';
                end = copy + (end - begin) + 1;
            } else {
                memcpy(copy, begin, end - begin);
                copy[0] = '0';
                copy[end - begin] = '\0';
                end = copy + (end - begin);
            }
            begin = copy;
        } else {
            // If there's a leading dot, copy the string and insert zero prior to the dot (e. g. '.15' -> '0.15')
            if (begin[0] == '.') {
                copy = new char[end - begin + 2];
                copy[0] = '0';
                memcpy(copy + 1, begin, end - begin);
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
        if (!copy && end - begin > 2 && begin[0] == '0') {
            const char prefix = begin[1];
            const char *sub = begin + 2;

            if (prefix == 'x' || prefix == 'X') {
                return value_hexToDec(sub, end - begin - 2, ok);
            } else if (prefix == 'o' || prefix == 'O') {
                return value_octToDec(sub, end - begin - 2, ok);
            } else if (prefix == 'b' || prefix == 'B') {
                return value_binToDec(sub, end - begin - 2, ok);
            }
        }

        // If there's a trailing dot, copy the string and append zero next to it (e. g. '1.' -> '1.0')
        if (end[-1] == '.') {
            if (copy) {
                char *tmpCopy = new char[end - begin + 2];
                memcpy(tmpCopy, begin, end - begin);
                delete[] copy;
                copy = tmpCopy;
                end = copy + (end - begin) + 1;
                begin = copy;
            } else {
                copy = new char[end - begin + 2];
                memcpy(copy, begin, end - begin);
                end = copy + (end - begin) + 1;
                begin = copy;
            }

            copy[end - begin - 1] = '0';
            copy[end - begin] = '\0';
        }

        // Trim leading zeros
        bool trimmed = false;

        while (begin < end && (*begin == '0') && !(begin + 1 < end && begin[1] == '.')) {
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
        if (trimmed && (begin[0] == '+' || begin[0] == '-')) {
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
        if (value_stringsEqual(s, "Infinity")) {
            if (ok)
                *ok = true;
            return std::numeric_limits<double>::infinity();
        } else if (value_stringsEqual(s, "-Infinity")) {
            if (ok)
                *ok = true;
            return -std::numeric_limits<double>::infinity();
        }

        return 0;
    }

    inline long value_stringToLong(const char *s, bool *ok = nullptr)
    {
        const double ret = value_stringToDoubleImpl(s, ok);

        if (std::isinf(ret))
            return 0;

        return ret;
    }

    inline bool value_stringIsInt(const char *s, int n)
    {
        const char *p = s;

        while (p < s + n) {
            if (*p == '.' || *p == 'e' || *p == 'E')
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

    inline int value_checkString(const char *str)
    {
        bool ok;

        if (value_stringIsInt(str, strlen(str))) {
            value_stringToLong(str, &ok);
            return ok ? 1 : 0;
        } else {
            value_stringToDoubleImpl(str, &ok);
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
            return value_stringToDoubleImpl(v->stringValue, ok);
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
            char *s1 = value_toCString(v1);
            char *s2 = value_toCString(v2);
            int ret = strcasecmp(s1, s2);
            free(s1);
            free(s2);
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
