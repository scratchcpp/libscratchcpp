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

template<typename T>
inline bool value_isInf(T v)
{
    return v > 0 && std::isinf(v);
}

template<typename T>
inline bool value_isNegativeInf(T v)
{
    return v < 0 && std::isinf(v);
}

extern "C"
{

    inline long value_convert_int_str(const char *str, int n, bool *ok)
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
            return -ret;
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

    inline double value_stringToDouble(const char *s, bool *ok = nullptr)
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
        } else
            return 0;

        // Special values
        if (strncmp(s, "Infinity", len) == 0) {
            if (ok)
                *ok = true;
            return std::numeric_limits<double>::infinity();
        } else if (strncmp(s, "-Infinity", len) == 0) {
            if (ok)
                *ok = true;
            return -std::numeric_limits<double>::infinity();
        } else if (strncmp(s, "NaN", len) == 0) {
            if (ok)
                *ok = true;
            return std::numeric_limits<double>::quiet_NaN();
        }

        return 0;
    }

    inline long value_stringToLong(const char *s, bool *ok = nullptr)
    {
        return value_stringToDouble(s, ok);
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

inline std::string value_doubleToString(double v)
{
    std::stringstream stream;

    if (v != 0) {
        const int exponent = std::log10(std::abs(v));

        if (exponent > 20)
            stream << std::scientific << std::setprecision(value_digitCount(v / std::pow(10, exponent + 1)) - 1) << v;
        else
            stream << std::setprecision(std::max(16u, value_digitCount(v))) << v;
    } else
        stream << std::setprecision(std::max(16u, value_digitCount(v))) << v;

    std::string s = stream.str();
    std::size_t index;

    for (int i = 0; i < 2; i++) {
        if (i == 0)
            index = s.find("e+");
        else
            index = s.find("e-");

        if (index != std::string::npos) {
            while ((s.size() >= index + 3) && (s[index + 2] == '0'))
                s.erase(index + 2, 1);
        }
    }

    return s;
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
            value_stringToDouble(str, &ok);
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
            return value_stringToDouble(v->stringValue, ok);
        else if (v->type == ValueType::Infinity)
            return std::numeric_limits<double>::infinity();
        else if (v->type == ValueType::NegativeInfinity)
            return -std::numeric_limits<double>::infinity();
        else if (v->type == ValueType::NaN) {
            if (ok)
                *ok = false;

            return 0;
        } else if (v->type == ValueType::Number)
            return v->numberValue;
        else if (v->type == ValueType::Bool)
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
        else if (static_cast<int>(v->type) < 0) {
            if (v->type == ValueType::Infinity)
                return true;
            else
                return false;
        }

        return value_toDouble(v) > 0;
    }

    bool value_isNegative(const ValueData *v)
    {
        if (v->type == ValueType::Number)
            return v->numberValue < 0;
        else if (v->type == ValueType::Bool)
            return false;
        else if (static_cast<int>(v->type) < 0) {
            if (v->type == ValueType::NegativeInfinity)
                return true;
            else
                return false;
        }

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
}

} // namespace libscratchcpp
