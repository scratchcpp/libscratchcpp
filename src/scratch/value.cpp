// SPDX-License-Identifier: Apache-2.0

#include "value.h"
#include <algorithm>
#include <limits>
#include <utf8.h>

using namespace libscratchcpp;

/*! Constructs a number Value. */
Value::Value(float numberValue) :
    m_value(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(double numberValue) :
    m_value(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(int numberValue) :
    m_value(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(long numberValue) :
    m_value(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a boolean Value. */
Value::Value(bool boolValue) :
    m_value(boolValue),
    m_type(Type::Bool)
{
}

/*! Constructs a string Value. */
Value::Value(const std::string &stringValue) :
    m_value(stringValue),
    m_type(Type::String)
{
    if (stringValue.empty())
        return;
    else if (stringValue == "Infinity") {
        m_isInfinity = true;
        m_type = Type::Special;
        return;
    } else if (stringValue == "-Infinity") {
        m_isNegativeInfinity = true;
        m_type = Type::Special;
        return;
    } else if (stringValue == "NaN") {
        m_isNaN = true;
        m_type = Type::Special;
        return;
    }

    bool ok;
    float f = stringToDouble(stringValue, &ok);
    if (ok) {
        m_value = f;
        m_type = Type::Number;
    }
}

/*! Constructs a string Value. */
Value::Value(const char *stringValue) :
    Value(std::string(stringValue))
{
}

/*! Constructs a special Value. */
Value::Value(SpecialValue specialValue) :
    m_type(Type::Special)
{
    if (specialValue == SpecialValue::Infinity)
        m_isInfinity = true;
    else if (specialValue == SpecialValue::NegativeInfinity)
        m_isNegativeInfinity = true;
    else if (specialValue == SpecialValue::NaN)
        m_isNaN = true;
    else {
        m_type = Type::Number;
        m_value = 0;
    }
}

/*! Returns the type of the value. */
Value::Type Value::type() const
{
    return m_type;
}

/*! Returns true if the value is infinity. */
bool Value::isInfinity() const
{
    return m_isInfinity;
}

/*! Returns true if the value is negative infinity. */
bool Value::isNegativeInfinity() const
{
    return m_isNegativeInfinity;
}

/*! Returns true if the value is NaN (Not a Number). */
bool Value::isNaN() const
{
    return m_isNaN;
}

/*! Returns true if the value is a number. */
bool Value::isNumber() const
{
    return m_type == Type::Number;
}

/*! Returns true if the value is a boolean. */
bool Value::isBool() const
{
    return m_type == Type::Bool;
}

/*! Returns true if the value is a string. */
bool Value::isString() const
{
    return m_type == Type::String;
}

/*! Returns the int representation of the value. */
int Value::toInt() const
{
    return toLong();
}

/*! Returns the long representation of the value. */
inline long Value::toLong() const
{
    if (std::holds_alternative<long>(m_value))
        return std::get<long>(m_value);
    else if (std::holds_alternative<double>(m_value))
        return std::get<double>(m_value);
    else if (std::holds_alternative<bool>(m_value))
        return std::get<bool>(m_value);
    else if (std::holds_alternative<std::string>(m_value))
        return stringToLong(std::get<std::string>(m_value));
    else if (m_isInfinity)
        return std::numeric_limits<long>::infinity();
    else if (m_isNegativeInfinity)
        return -std::numeric_limits<long>::infinity();
    else
        return 0;
}

/*! Returns the double representation of the value. */
double Value::toDouble() const
{
    if (std::holds_alternative<double>(m_value))
        return std::get<double>(m_value);
    else if (std::holds_alternative<long>(m_value))
        return std::get<long>(m_value);
    else if (std::holds_alternative<bool>(m_value))
        return std::get<bool>(m_value);
    else if (std::holds_alternative<std::string>(m_value))
        return stringToDouble(std::get<std::string>(m_value));
    else if (m_isInfinity)
        return std::numeric_limits<double>::infinity();
    else if (m_isNegativeInfinity)
        return -std::numeric_limits<double>::infinity();
    else
        return 0;
}

/*! Returns the boolean representation of the value. */
bool Value::toBool() const
{
    if (std::holds_alternative<bool>(m_value))
        return std::get<bool>(m_value);
    else if (std::holds_alternative<long>(m_value))
        return std::get<long>(m_value) == 1 ? true : false;
    else if (std::holds_alternative<double>(m_value))
        return std::get<double>(m_value) == 1 ? true : false;
    else if (std::holds_alternative<std::string>(m_value)) {
        const std::string &str = std::get<std::string>(m_value);
        return (stringsEqual(str, "true") || str == "1");
    } else
        return false;
}

/*! Returns the string representation of the value. */
std::string Value::toString() const
{
    if (std::holds_alternative<std::string>(m_value))
        return std::get<std::string>(m_value);
    else if (std::holds_alternative<long>(m_value))
        return std::to_string(std::get<long>(m_value));
    else if (std::holds_alternative<double>(m_value)) {
        std::string s = std::to_string(std::get<double>(m_value));
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') {
            s.pop_back();
        }
        return s;
    } else if (std::holds_alternative<bool>(m_value))
        return std::get<bool>(m_value) ? "true" : "false";
    else if (m_isInfinity)
        return "Infinity";
    else if (m_isNegativeInfinity)
        return "-Infinity";
    else
        return "NaN";
}

/*! Returns the UTF-16 representation of the value. */
std::u16string Value::toUtf16() const
{
    return utf8::utf8to16(toString());
}

bool Value::stringsEqual(std::u16string s1, std::u16string s2)
{
    std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    return (s1.compare(s2) == 0);
}

bool Value::stringsEqual(std::string s1, std::string s2)
{
    std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    return (s1.compare(s2) == 0);
}

double Value::stringToDouble(const std::string &s, bool *ok)
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

    static const std::string digits = "0123456789.eE+-";
    for (char c : s) {
        if (digits.find(c) == std::string::npos) {
            return 0;
        }
    }
    try {
        if (ok)
            *ok = true;
        return std::stof(s);
    } catch (...) {
        return 0;
    }
}

long Value::stringToLong(const std::string &s, bool *ok)
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

    try {
        if (ok)
            *ok = true;
        return std::stol(s);
    } catch (...) {
        return 0;
    }
}
