// SPDX-License-Identifier: Apache-2.0

#include "value.h"
#include <algorithm>
#include <limits>
#include <utf8.h>

using namespace libscratchcpp;

/*! Constructs a number Value. */
Value::Value(float numberValue) :
    m_numberValue(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a number Value. */
Value::Value(double numberValue) :
    m_numberValue(numberValue),
    m_type(Type::Number)
{
}

/*! Constructs a boolean Value. */
Value::Value(bool boolValue) :
    m_boolValue(boolValue),
    m_type(Type::Bool)
{
}

/*! Constructs a string Value. */
Value::Value(std::string stringValue) :
    m_stringValue(stringValue),
    m_type(Type::String)
{
    if (m_stringValue.empty())
        return;
    bool ok;
    float f = stringToFloat(m_stringValue, &ok);
    if (ok) {
        m_numberValue = f;
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
    else if (specialValue == SpecialValue::NaN)
        m_isNaN = true;
    else {
        m_type = Type::Number;
        m_numberValue = 0;
    }
}

/*! Returns the type of the value. */
Value::Type Value::type() const
{
    return m_type;
}

/*! Returns true if the value is not finite. */
bool Value::isInfinity() const
{
    return m_isInfinity;
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

/*! Returns the number representation of the value. */
float Value::toNumber() const
{
    switch (m_type) {
        case Type::Number:
            return m_numberValue;
        case Type::Bool:
            return m_boolValue;
        case Type::String:
            return stringToFloat(m_stringValue);
        case Type::Special:
            if (m_isInfinity)
                return std::numeric_limits<float>::infinity();
            else
                return 0;
    }
    return 0;
}

/*! Returns the boolean representation of the value. */
bool Value::toBool() const
{
    switch (m_type) {
        case Type::Number:
            return m_numberValue == 1 ? true : false;
        case Type::Bool:
            return m_boolValue;
        case Type::String: {
            std::string str = m_stringValue;
            return (stringsEqual(str, "true") || str == "1");
        }
        case Type::Special:
            return false;
    }
    return false;
}

/*! Returns the string representation of the value. */
std::string Value::toString() const
{
    switch (m_type) {
        case Type::Number: {
            std::string s = std::to_string(m_numberValue);
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            if (s.back() == '.') {
                s.pop_back();
            }
            return s;
        }
        case Type::Bool:
            return m_boolValue ? "true" : "false";
        case Type::String:
            return m_stringValue;
        case Type::Special:
            if (m_isInfinity)
                return "Infinity";
            else
                return "NaN";
    }
    return "";
}

/*! Returns the UTF-16 representation of the value. */
std::u16string Value::toUtf16() const
{
    return utf8::utf8to16(toString());
}

bool Value::stringsEqual(std::u16string s1, std::u16string s2)
{
    std::transform(s1.begin(), s1.end(), s2.begin(), ::tolower);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    return (s1.compare(s2) == 0);
}

bool Value::stringsEqual(std::string s1, std::string s2)
{
    std::transform(s1.begin(), s1.end(), s2.begin(), ::tolower);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    return (s1.compare(s2) == 0);
}

float Value::stringToFloat(std::string s, bool *ok)
{
    if (ok)
        *ok = false;
    std::string digits = "0123456789.eE+-";
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
