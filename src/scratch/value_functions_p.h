// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value_functions.h>
#include <algorithm>
#include <limits>
#include <ctgmath>
#include <iomanip>
#include <cassert>

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

    inline bool value_u16StringsEqual(std::u16string s1, std::u16string s2)
    {
        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        return (s1.compare(s2) == 0);
    }

    inline bool value_stringsEqual(std::string s1, std::string s2)
    {
        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        return (s1.compare(s2) == 0);
    }

    inline long value_hexToDec(const std::string &s)
    {
        static const std::string digits = "0123456789abcdef";

        for (char c : s) {
            if (digits.find(c) == std::string::npos) {
                return 0;
            }
        }

        std::istringstream stream(s);
        long ret;
        stream >> std::hex >> ret;
        return ret;
    }

    inline long value_octToDec(const std::string &s)
    {
        static const std::string digits = "01234567";

        for (char c : s) {
            if (digits.find(c) == std::string::npos) {
                return 0;
            }
        }

        std::istringstream stream(s);
        long ret;
        stream >> std::oct >> ret;
        return ret;
    }

    inline double value_binToDec(const std::string &s)
    {
        static const std::string digits = "01";

        for (char c : s) {
            if (digits.find(c) == std::string::npos) {
                return 0;
            }
        }

        return std::stoi(s, 0, 2);
    }

    inline double value_stringToDouble(const std::string &s, bool *ok = nullptr)
    {
        if (ok)
            *ok = false;

        if (s == "Infinity") {
            if (ok)
                *ok = true;
            return std::numeric_limits<double>::infinity();
        } else if (s == "-Infinity") {
            if (ok)
                *ok = true;
            return -std::numeric_limits<double>::infinity();
        } else if (s == "NaN") {
            if (ok)
                *ok = true;
            return 0;
        }

        if (s.size() >= 2 && s[0] == '0') {
            std::string sub = s.substr(2, s.size() - 2);
            std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);

            if (s[1] == 'x' || s[1] == 'X') {
                return value_hexToDec(sub);
            } else if (s[1] == 'o' || s[1] == 'O') {
                return value_octToDec(sub);
            } else if (s[1] == 'b' || s[1] == 'B') {
                return value_binToDec(sub);
            }
        }

        static const std::string digits = "0123456789.eE+-";
        const std::string *stringPtr = &s;
        bool customStr = false;

        if (!s.empty() && ((s[0] == ' ') || (s.back() == ' '))) {
            std::string *localPtr = new std::string(s);
            stringPtr = localPtr;
            customStr = true;

            while (!localPtr->empty() && (localPtr->at(0) == ' '))
                localPtr->erase(0, 1);

            while (!localPtr->empty() && (localPtr->back() == ' '))
                localPtr->pop_back();
        }

        for (char c : *stringPtr) {
            if (digits.find(c) == std::string::npos) {
                return 0;
            }
        }

        try {
            if (ok)
                *ok = true;

            // Set locale to C to avoid conversion issues
            std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
            std::setlocale(LC_NUMERIC, "C");

            double ret = std::stod(*stringPtr);

            // Restore old locale
            std::setlocale(LC_NUMERIC, oldLocale.c_str());

            if (customStr)
                delete stringPtr;

            return ret;
        } catch (...) {
            if (ok)
                *ok = false;
            return 0;
        }
    }

    inline long value_stringToLong(const std::string &s, bool *ok = nullptr)
    {
        if (ok)
            *ok = false;

        if (s == "Infinity") {
            if (ok)
                *ok = true;
            return std::numeric_limits<long>::infinity();
        } else if (s == "-Infinity") {
            if (ok)
                *ok = true;
            return -std::numeric_limits<long>::infinity();
        } else if (s == "NaN") {
            if (ok)
                *ok = true;
            return 0;
        }

        if (s.size() >= 2 && s[0] == '0') {
            std::string sub = s.substr(2, s.size() - 2);
            std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);

            if (s[1] == 'x' || s[1] == 'X') {
                return value_hexToDec(sub);
            } else if (s[1] == 'o' || s[1] == 'O') {
                return value_octToDec(sub);
            } else if (s[1] == 'b' || s[1] == 'B') {
                return value_binToDec(sub);
            }
        }

        static const std::string digits = "0123456789.eE+-";

        for (char c : s) {
            if (digits.find(c) == std::string::npos) {
                return 0;
            }
        }

        try {
            if (ok)
                *ok = true;
            return std::stol(s);
        } catch (...) {
            if (ok)
                *ok = false;
            return 0;
        }
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

    inline int value_checkString(const std::string &str)
    {
        bool ok;

        if ((str.find_first_of('.') == std::string::npos) && (str.find_first_of('e') == std::string::npos) && (str.find_first_of('E') == std::string::npos)) {
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
            return value_stringToDouble(*v->stringValue, ok);
        else if (v->type == ValueType::Infinity)
            return std::numeric_limits<double>::infinity();
        else if (v->type == ValueType::NegativeInfinity)
            return -std::numeric_limits<double>::infinity();
        else if (v->type == ValueType::NaN) {
            if (ok)
                *ok = false;

            return 0;
        } else if (v->type == ValueType::Integer)
            return v->intValue;
        else if (v->type == ValueType::Double)
            return v->doubleValue;
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
        if (v->type == ValueType::Integer)
            return v->intValue > 0;
        else if (v->type == ValueType::Double)
            return v->doubleValue > 0;
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
        if (v->type == ValueType::Integer)
            return v->intValue < 0;
        else if (v->type == ValueType::Double)
            return v->doubleValue < 0;
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
        if (v->type == ValueType::Integer)
            return v->intValue == 0;
        else if (v->type == ValueType::Double)
            return v->doubleValue == 0;
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
