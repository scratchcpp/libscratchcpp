// SPDX-License-Identifier: Apache-2.0

#include "value.h"
#include <algorithm>
#include <limits>

using namespace libscratchcpp;

/*! Constructs a number Value. */
Value::Value(float numberValue) :
    ValueVariant(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(double numberValue) :
    ValueVariant(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(int numberValue) :
    ValueVariant(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(size_t numberValue) :
    ValueVariant(static_cast<long>(numberValue)),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(long numberValue) :
    ValueVariant(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a boolean Value. */
Value::Value(bool boolValue) :
    ValueVariant(boolValue),
    m_type(Type::Bool)
{
}

/*! Constructs a string Value. */
Value::Value(const std::string &stringValue) :
    ValueVariant(stringValue),
    m_type(Type::String)
{
    initString(stringValue);
}

/*! Constructs a string Value. */
Value::Value(const char *stringValue) :
    Value(std::string(stringValue))
{
}

/*! Constructs a special Value. */
Value::Value(SpecialValue specialValue) :
    ValueVariant(0)
{
    if (specialValue == SpecialValue::Infinity)
        m_type = Type::Infinity;
    else if (specialValue == SpecialValue::NegativeInfinity)
        m_type = Type::NegativeInfinity;
    else if (specialValue == SpecialValue::NaN)
        m_type = Type::NaN;
    else
        m_type = Type::Number;
}

/*! Returns the type of the value. */
Value::Type Value::type() const
{
    return m_type;
}

/*! Returns true if the value is infinity. */
bool Value::isInfinity() const
{
    return m_type == Type::Infinity;
}

/*! Returns true if the value is negative infinity. */
bool Value::isNegativeInfinity() const
{
    return m_type == Type::NegativeInfinity;
}

/*! Returns true if the value is NaN (Not a Number). */
bool Value::isNaN() const
{
    return m_type == Type::NaN;
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
