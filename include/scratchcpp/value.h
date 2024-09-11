// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <algorithm>
#include <limits>
#include <ctgmath>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <clocale>

#include "global.h"

namespace libscratchcpp
{

enum class ValueType
{
    Integer = 0,
    Double = 1,
    Bool = 2,
    String = 3,
    Infinity = -1,
    NegativeInfinity = -2,
    NaN = -3
};

extern "C"
{
    /*! \brief The ValueData struct holds the data of Value. It's used in compiled Scratch code for better performance. */
    struct ValueData
    {
            union
            {
                    long intValue;
                    double doubleValue;
                    bool boolValue;
                    std::string *stringValue;
            };

            ValueType type;
    };
}

/*! \brief The Value class represents a Scratch value. */
class LIBSCRATCHCPP_EXPORT Value
{
    public:
        enum class SpecialValue
        {
            Infinity,
            NegativeInfinity,
            NaN
        };

        /*! Constructs a number Value. */
        Value(float numberValue)
        {
            if (isInf(numberValue))
                m_data.type = ValueType::Infinity;
            else if (isNegativeInf(numberValue))
                m_data.type = ValueType::NegativeInfinity;
            else if (std::isnan(numberValue))
                m_data.type = ValueType::NaN;
            else {
                m_data.type = ValueType::Double;
                m_data.doubleValue = floatToDouble(numberValue);
            }
        }

        /*! Constructs a number Value. */
        Value(double numberValue)
        {
            if (isInf(numberValue))
                m_data.type = ValueType::Infinity;
            else if (isNegativeInf(numberValue))
                m_data.type = ValueType::NegativeInfinity;
            else if (std::isnan(numberValue))
                m_data.type = ValueType::NaN;
            else {
                m_data.type = ValueType::Double;
                m_data.doubleValue = numberValue;
            }
        }

        /*! Constructs a number Value. */
        Value(int numberValue = 0)
        {
            m_data.type = ValueType::Integer;
            m_data.intValue = numberValue;
        }

        /*! Constructs a number Value. */
        Value(size_t numberValue)
        {
            m_data.type = ValueType::Integer;
            m_data.intValue = numberValue;
        }

        /*! Constructs a number Value. */
        Value(long numberValue)
        {
            m_data.type = ValueType::Integer;
            m_data.intValue = numberValue;
        }

        /*! Constructs a boolean Value. */
        Value(bool boolValue)
        {
            m_data.type = ValueType::Bool;
            m_data.boolValue = boolValue;
        }

        /*! Constructs a string Value. */
        Value(const std::string &stringValue)
        {
            if (stringValue == "Infinity")
                m_data.type = ValueType::Infinity;
            else if (stringValue == "-Infinity")
                m_data.type = ValueType::NegativeInfinity;
            else if (stringValue == "NaN")
                m_data.type = ValueType::NaN;
            else {
                m_data.type = ValueType::String;
                m_data.stringValue = new std::string(stringValue);
            }
        }

        /*! Constructs a string Value. */
        Value(const char *stringValue) :
            Value(std::string(stringValue))
        {
        }

        /*! Constructs a special Value. */
        Value(SpecialValue specialValue)
        {
            if (specialValue == SpecialValue::Infinity)
                m_data.type = ValueType::Infinity;
            else if (specialValue == SpecialValue::NegativeInfinity)
                m_data.type = ValueType::NegativeInfinity;
            else if (specialValue == SpecialValue::NaN)
                m_data.type = ValueType::NaN;
            else {
                m_data.type = ValueType::Integer;
                m_data.intValue = 0;
            }
        }

        Value(const Value &v)
        {
            m_data.type = v.m_data.type;

            switch (m_data.type) {
                case ValueType::Integer:
                    m_data.intValue = v.m_data.intValue;
                    break;
                case ValueType::Double:
                    m_data.doubleValue = v.m_data.doubleValue;
                    break;
                case ValueType::Bool:
                    m_data.boolValue = v.m_data.boolValue;
                    break;
                case ValueType::String:
                    m_data.stringValue = new std::string(*v.m_data.stringValue);
                    break;
                default:
                    break;
            }
        }

        ~Value()
        {
            if (m_data.type == ValueType::String)
                delete m_data.stringValue;
        }

        /*! Returns the type of the value. */
        ValueType type() const { return m_data.type; }

        /*! Returns true if the value is infinity. */
        bool isInfinity() const
        {
            switch (m_data.type) {
                case ValueType::Infinity:
                    return true;
                case ValueType::Integer:
                    return isInf(m_data.intValue);
                case ValueType::Double:
                    return isInf(m_data.doubleValue);
                case ValueType::String:
                    return *m_data.stringValue == "Infinity";
                default:
                    return false;
            }
        }

        /*! Returns true if the value is negative infinity. */
        bool isNegativeInfinity() const
        {
            switch (m_data.type) {
                case ValueType::NegativeInfinity:
                    return true;
                case ValueType::Integer:
                    return isNegativeInf(-m_data.intValue);
                case ValueType::Double:
                    return isNegativeInf(-m_data.doubleValue);
                case ValueType::String:
                    return *m_data.stringValue == "-Infinity";
                default:
                    return false;
            }
        }

        /*! Returns true if the value is NaN (Not a Number). */
        bool isNaN() const
        {
            switch (m_data.type) {
                case ValueType::NaN:
                    return true;
                case ValueType::Double:
                    assert(!std::isnan(m_data.doubleValue));
                    return std::isnan(m_data.doubleValue);
                case ValueType::String:
                    return *m_data.stringValue == "NaN";
                default:
                    return false;
            }
        }

        /*! Returns true if the value is a number. */
        bool isNumber() const { return m_data.type == ValueType::Integer || m_data.type == ValueType::Double; }

        /*! Returns true if the value is a number or can be converted to a number. */
        bool isValidNumber() const
        {
            if (isNaN())
                return false;

            if (isInfinity() || isNegativeInfinity())
                return true;

            assert(m_data.type != ValueType::Infinity && m_data.type != ValueType::NegativeInfinity);

            switch (m_data.type) {
                case ValueType::Integer:
                case ValueType::Double:
                case ValueType::Bool:
                    return true;
                case ValueType::String:
                    return m_data.stringValue->empty() || checkString(*m_data.stringValue) > 0;
                default:
                    return false;
            }
        }

        /*! Returns true if this value represents a round integer. */
        bool isInt() const
        {
            // https://github.com/scratchfoundation/scratch-vm/blob/112989da0e7306eeb405a5c52616e41c2164af24/src/util/cast.js#L157-L181
            switch (m_data.type) {
                case ValueType::Integer:
                case ValueType::Bool:
                case ValueType::Infinity:
                case ValueType::NegativeInfinity:
                case ValueType::NaN:
                    return true;
                case ValueType::Double: {
                    double intpart;
                    std::modf(m_data.doubleValue, &intpart);
                    return m_data.doubleValue == intpart;
                }
                case ValueType::String:
                    return m_data.stringValue->find('.') == std::string::npos;
            }

            return false;
        }

        /*! Returns true if the value is a boolean. */
        bool isBool() const { return m_data.type == ValueType::Bool; }

        /*! Returns true if the value is a string. */
        bool isString() const { return m_data.type == ValueType::String; }

        /*! Returns the int representation of the value. */
        int toInt() const { return toLong(); }

        /*! Returns the long representation of the value. */
        long toLong() const
        {
            if (static_cast<int>(m_data.type) < 0)
                return 0;
            else {
                switch (m_data.type) {
                    case ValueType::Integer:
                        return m_data.intValue;
                    case ValueType::Double:
                        return m_data.doubleValue;
                    case ValueType::Bool:
                        return m_data.boolValue;
                    case ValueType::String:
                        return stringToLong(*m_data.stringValue);
                    default:
                        return 0;
                }
            }
        }

        /*! Returns the double representation of the value. */
        double toDouble() const
        {
            if (static_cast<int>(m_data.type) < 0) {
                switch (m_data.type) {
                    case ValueType::Infinity:
                        return std::numeric_limits<double>::infinity();
                    case ValueType::NegativeInfinity:
                        return -std::numeric_limits<double>::infinity();
                    default:
                        return 0;
                }
            } else {
                switch (m_data.type) {
                    case ValueType::Double:
                        return m_data.doubleValue;
                    case ValueType::Integer:
                        return m_data.intValue;
                    case ValueType::Bool:
                        return m_data.boolValue;
                    case ValueType::String:
                        return stringToDouble(*m_data.stringValue);
                    default:
                        return 0;
                }
            }
        };

        /*! Returns the boolean representation of the value. */
        bool toBool() const
        {
            switch (m_data.type) {
                case ValueType::Bool:
                    return m_data.boolValue;
                case ValueType::Integer:
                    return m_data.intValue != 0;
                case ValueType::Double:
                    return m_data.doubleValue != 0;
                case ValueType::String:
                    return !m_data.stringValue->empty() && !stringsEqual(*m_data.stringValue, "false") && *m_data.stringValue != "0";
                case ValueType::Infinity:
                case ValueType::NegativeInfinity:
                    return true;
                case ValueType::NaN:
                    return false;
                default:
                    return false;
            }
        };

        /*! Returns the string representation of the value. */
        std::string toString() const
        {
            if (static_cast<int>(m_data.type) < 0) {
                switch (m_data.type) {
                    case ValueType::Infinity:
                        return "Infinity";
                    case ValueType::NegativeInfinity:
                        return "-Infinity";
                    default:
                        return "NaN";
                }
            } else {
                switch (m_data.type) {
                    case ValueType::String:
                        return *m_data.stringValue;
                    case ValueType::Integer:
                        return std::to_string(m_data.intValue);
                    case ValueType::Double: {
                        return doubleToString(m_data.doubleValue);
                    }
                    case ValueType::Bool:
                        return m_data.boolValue ? "true" : "false";
                    default:
                        return "";
                }
            }
        };

        /*! Returns the UTF-16 representation of the value. */
        std::u16string toUtf16() const;

        /*! Adds the given value to the value. */
        void add(const Value &v)
        {
            if ((static_cast<int>(m_data.type) < 0) || (static_cast<int>(v.m_data.type) < 0)) {
                if ((m_data.type == ValueType::Infinity && v.m_data.type == ValueType::NegativeInfinity) || (m_data.type == ValueType::NegativeInfinity && v.m_data.type == ValueType::Infinity))
                    m_data.type = ValueType::NaN;
                else if (m_data.type == ValueType::Infinity || v.m_data.type == ValueType::Infinity)
                    m_data.type = ValueType::Infinity;
                else if (m_data.type == ValueType::NegativeInfinity || v.m_data.type == ValueType::NegativeInfinity)
                    m_data.type = ValueType::NegativeInfinity;
                return;
            }

            if (m_data.type == ValueType::Integer && v.m_data.type == ValueType::Integer)
                m_data.intValue += v.m_data.intValue;
            else
                *this = toDouble() + v.toDouble();
        }

        /*! Subtracts the given value from the value. */
        void subtract(const Value &v)
        {
            if ((static_cast<int>(m_data.type) < 0) || (static_cast<int>(v.m_data.type) < 0)) {
                if ((m_data.type == ValueType::Infinity && v.m_data.type == ValueType::Infinity) || (m_data.type == ValueType::NegativeInfinity && v.m_data.type == ValueType::NegativeInfinity))
                    m_data.type = ValueType::NaN;
                else if (m_data.type == ValueType::Infinity || v.m_data.type == ValueType::NegativeInfinity)
                    m_data.type = ValueType::Infinity;
                else if (m_data.type == ValueType::NegativeInfinity || v.m_data.type == ValueType::Infinity)
                    m_data.type = ValueType::NegativeInfinity;
                return;
            }

            if (m_data.type == ValueType::Integer && v.m_data.type == ValueType::Integer)
                m_data.intValue -= v.m_data.intValue;
            else
                *this = toDouble() - v.toDouble();
        }

        /*! Multiplies the given value with the value. */
        void multiply(const Value &v)
        {
            ValueType t1 = m_data.type, t2 = v.m_data.type;
            if ((static_cast<int>(t1) < 0 && t1 != ValueType::NaN) || (static_cast<int>(t2) < 0 && t2 != ValueType::NaN)) {
                if (t1 == ValueType::Infinity || t1 == ValueType::NegativeInfinity || t2 == ValueType::Infinity || t2 == ValueType::NegativeInfinity) {
                    bool mode = (t1 == ValueType::Infinity || t2 == ValueType::Infinity);
                    const Value &value =
                        ((t1 == ValueType::Infinity && (t2 == ValueType::Infinity || t2 == ValueType::NegativeInfinity)) || (t2 != ValueType::Infinity && t2 != ValueType::NegativeInfinity)) ?
                            v :
                            *this;
                    if (value > 0)
                        m_data.type = mode ? ValueType::Infinity : ValueType::NegativeInfinity;
                    else if (value < 0)
                        m_data.type = mode ? ValueType::NegativeInfinity : ValueType::Infinity;
                    else
                        m_data.type = ValueType::NaN;
                    return;
                }
            }

            if (m_data.type == ValueType::Integer && v.m_data.type == ValueType::Integer)
                m_data.intValue *= v.m_data.intValue;
            else
                *this = toDouble() * v.toDouble();
        }

        /*! Divides the value by the given value. */
        void divide(const Value &v)
        {
            if ((toDouble() == 0) && (v.toDouble() == 0)) {
                m_data.type = ValueType::NaN;
                return;
            } else if (v.toDouble() == 0) {
                m_data.type = *this > 0 ? ValueType::Infinity : ValueType::NegativeInfinity;
                return;
            } else if ((m_data.type == ValueType::Infinity || m_data.type == ValueType::NegativeInfinity) && (v.m_data.type == ValueType::Infinity || v.m_data.type == ValueType::NegativeInfinity)) {
                m_data.type = ValueType::NaN;
                return;
            } else if (m_data.type == ValueType::Infinity || m_data.type == ValueType::NegativeInfinity) {
                if (v.toDouble() < 0)
                    m_data.type = m_data.type == ValueType::Infinity ? ValueType::NegativeInfinity : ValueType::Infinity;
                return;
            } else if (v.m_data.type == ValueType::Infinity || v.m_data.type == ValueType::NegativeInfinity) {
                *this = 0;
                return;
            }
            *this = toDouble() / v.toDouble();
        }

        /*! Replaces the value with modulo of the value and the given value. */
        void mod(const Value &v)
        {
            if ((v == 0) || (m_data.type == ValueType::Infinity || m_data.type == ValueType::NegativeInfinity)) {
                m_data.type = ValueType::NaN;
                return;
            } else if (v.m_data.type == ValueType::Infinity || v.m_data.type == ValueType::NegativeInfinity) {
                return;
            }
            if (*this < 0 || v < 0)
                *this = fmod(v.toDouble() + fmod(toDouble(), -v.toDouble()), v.toDouble());
            else
                *this = fmod(toDouble(), v.toDouble());
        }

        const Value &operator=(float v)
        {
            if (m_data.type == ValueType::String)
                delete m_data.stringValue;

            if (isInf(v))
                m_data.type = ValueType::Infinity;
            else if (isNegativeInf(v))
                m_data.type = ValueType::NegativeInfinity;
            else if (std::isnan(v))
                m_data.type = ValueType::NaN;
            else {
                m_data.type = ValueType::Double;
                m_data.doubleValue = floatToDouble(v);
            }

            return *this;
        }

        const Value &operator=(double v)
        {
            if (m_data.type == ValueType::String)
                delete m_data.stringValue;

            if (isInf(v))
                m_data.type = ValueType::Infinity;
            else if (isNegativeInf(v))
                m_data.type = ValueType::NegativeInfinity;
            else if (std::isnan(v))
                m_data.type = ValueType::NaN;
            else {
                m_data.type = ValueType::Double;
                m_data.doubleValue = v;
            }
            return *this;
        }

        const Value &operator=(int v)
        {
            if (m_data.type == ValueType::String)
                delete m_data.stringValue;

            m_data.type = ValueType::Integer;
            m_data.intValue = v;
            return *this;
        }

        const Value &operator=(long v)
        {
            if (m_data.type == ValueType::String)
                delete m_data.stringValue;

            m_data.type = ValueType::Integer;
            m_data.intValue = v;
            return *this;
        }

        const Value &operator=(bool v)
        {
            if (m_data.type == ValueType::String)
                delete m_data.stringValue;

            m_data.type = ValueType::Bool;
            m_data.boolValue = v;
            return *this;
        }

        const Value &operator=(const std::string &v)
        {
            if (v == "Infinity") {
                if (m_data.type == ValueType::String)
                    delete m_data.stringValue;

                m_data.type = ValueType::Infinity;
            } else if (v == "-Infinity") {
                if (m_data.type == ValueType::String)
                    delete m_data.stringValue;

                m_data.type = ValueType::NegativeInfinity;
            } else if (v == "NaN") {
                if (m_data.type == ValueType::String)
                    delete m_data.stringValue;

                m_data.type = ValueType::NaN;
            } else if (m_data.type == ValueType::String)
                m_data.stringValue->assign(v);
            else {
                m_data.stringValue = new std::string(v);
                m_data.type = ValueType::String;
            }

            return *this;
        }

        const Value &operator=(const char *v) { return (*this = std::string(v)); }

        const Value &operator=(const Value &v)
        {
            switch (v.m_data.type) {
                case ValueType::Integer:
                    m_data.intValue = v.m_data.intValue;
                    break;

                case ValueType::Double:
                    m_data.doubleValue = v.m_data.doubleValue;
                    break;

                case ValueType::Bool:
                    m_data.boolValue = v.m_data.boolValue;
                    break;

                case ValueType::String:
                    m_data.stringValue = new std::string(*v.m_data.stringValue);
                    break;

                default:
                    break;
            }

            m_data.type = v.m_data.type;

            return *this;
        }

    private:
        ValueData m_data;

        // 0 - is string
        // 1 - is long
        // 2 - is double
        static int checkString(const std::string &str)
        {
            bool ok;

            if ((str.find_first_of('.') == std::string::npos) && (str.find_first_of('e') == std::string::npos) && (str.find_first_of('E') == std::string::npos)) {
                stringToLong(str, &ok);
                return ok ? 1 : 0;
            } else {
                stringToDouble(str, &ok);
                return ok ? 2 : 0;
            }
        }

        double getNumber(bool *ok = nullptr) const
        {
            // Equivalent to JavaScript Number(), *ok == false means NaN
            if (ok)
                *ok = true;

            switch (m_data.type) {
                case ValueType::Integer:
                    return m_data.intValue;

                case ValueType::Double:
                    return m_data.doubleValue;

                case ValueType::Bool:
                    return m_data.boolValue;

                case ValueType::String:
                    return stringToDouble(*m_data.stringValue, ok);

                case ValueType::Infinity:
                    return std::numeric_limits<double>::infinity();

                case ValueType::NegativeInfinity:
                    return -std::numeric_limits<double>::infinity();

                case ValueType::NaN:
                    if (ok)
                        *ok = false;

                    return 0;

                default:
                    assert(false); // this should never happen
                    if (ok)
                        *ok = false;

                    return 0;
            }
        }

        friend bool operator==(const Value &v1, const Value &v2)
        {
            // https://github.com/scratchfoundation/scratch-vm/blob/112989da0e7306eeb405a5c52616e41c2164af24/src/util/cast.js#L121-L150
            if (v1.m_data.type == ValueType::Integer && v2.m_data.type == ValueType::Integer)
                return v1.m_data.intValue == v2.m_data.intValue;
            else if (v1.m_data.type == ValueType::Double && v2.m_data.type == ValueType::Double)
                return v1.m_data.doubleValue == v2.m_data.doubleValue;

            bool ok;
            double n1 = v1.getNumber(&ok);
            double n2;

            if (ok)
                n2 = v2.getNumber(&ok);

            if (!ok) {
                // At least one argument can't be converted to a number
                // Scratch compares strings as case insensitive
                return stringsEqual(v1.toUtf16(), v2.toUtf16());
            }

            // Handle the special case of Infinity
            if ((static_cast<int>(v1.m_data.type) < 0) && (static_cast<int>(v2.m_data.type) < 0)) {
                assert(v1.m_data.type != ValueType::NaN);
                assert(v2.m_data.type != ValueType::NaN);
                return v1.m_data.type == v2.m_data.type;
            }

            // Compare as numbers
            return n1 == n2;
        }

        friend bool operator!=(const Value &v1, const Value &v2) { return !(v1 == v2); }

        friend bool operator>(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_data.type) < 0) || (static_cast<int>(v2.m_data.type) < 0)) {
                if (v1.m_data.type == ValueType::Infinity) {
                    return v2.m_data.type != ValueType::Infinity;
                } else if (v1.m_data.type == ValueType::NegativeInfinity)
                    return false;
                else if (v2.m_data.type == ValueType::Infinity)
                    return false;
                else if (v2.m_data.type == ValueType::NegativeInfinity)
                    return true;
            }

            if (v1.m_data.type == ValueType::Integer && v2.m_data.type == ValueType::Integer)
                return v1.m_data.intValue > v2.m_data.intValue;
            else
                return v1.toDouble() > v2.toDouble();
        }

        friend bool operator<(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_data.type) < 0) || (static_cast<int>(v2.m_data.type) < 0)) {
                if (v1.m_data.type == ValueType::Infinity) {
                    return false;
                } else if (v1.m_data.type == ValueType::NegativeInfinity)
                    return v2.m_data.type != ValueType::NegativeInfinity;
                else if (v2.m_data.type == ValueType::Infinity)
                    return v1.m_data.type != ValueType::Infinity;
                else if (v2.m_data.type == ValueType::NegativeInfinity)
                    return false;
            }

            if (v1.m_data.type == ValueType::Integer && v2.m_data.type == ValueType::Integer)
                return v1.m_data.intValue < v2.m_data.intValue;
            else
                return v1.toDouble() < v2.toDouble();
        }

        friend bool operator>=(const Value &v1, const Value &v2) { return v1 > v2 || v1 == v2; }

        friend bool operator<=(const Value &v1, const Value &v2) { return v1 < v2 || v1 == v2; }

        friend Value operator+(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_data.type) < 0) || (static_cast<int>(v2.m_data.type) < 0)) {
                if ((v1.m_data.type == ValueType::Infinity && v2.m_data.type == ValueType::NegativeInfinity) ||
                    (v1.m_data.type == ValueType::NegativeInfinity && v2.m_data.type == ValueType::Infinity))
                    return Value(SpecialValue::NaN);
                else if (v1.m_data.type == ValueType::Infinity || v2.m_data.type == ValueType::Infinity)
                    return Value(SpecialValue::Infinity);
                else if (v1.m_data.type == ValueType::NegativeInfinity || v2.m_data.type == ValueType::NegativeInfinity)
                    return Value(SpecialValue::NegativeInfinity);
            }

            if (v1.m_data.type == ValueType::Integer && v2.m_data.type == ValueType::Integer)
                return v1.m_data.intValue + v2.m_data.intValue;
            else
                return v1.toDouble() + v2.toDouble();
        }

        friend Value operator-(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_data.type) < 0) || (static_cast<int>(v2.m_data.type) < 0)) {
                if ((v1.m_data.type == ValueType::Infinity && v2.m_data.type == ValueType::Infinity) ||
                    (v1.m_data.type == ValueType::NegativeInfinity && v2.m_data.type == ValueType::NegativeInfinity))
                    return Value(SpecialValue::NaN);
                else if (v1.m_data.type == ValueType::Infinity || v2.m_data.type == ValueType::NegativeInfinity)
                    return Value(SpecialValue::Infinity);
                else if (v1.m_data.type == ValueType::NegativeInfinity || v2.m_data.type == ValueType::Infinity)
                    return Value(SpecialValue::NegativeInfinity);
            }

            if (v1.m_data.type == ValueType::Integer && v2.m_data.type == ValueType::Integer)
                return v1.m_data.intValue - v2.m_data.intValue;
            else
                return v1.toDouble() - v2.toDouble();
        }

        friend Value operator*(const Value &v1, const Value &v2)
        {
            ValueType t1 = v1.m_data.type, t2 = v2.m_data.type;
            if ((static_cast<int>(t1) < 0 && t1 != ValueType::NaN) || (static_cast<int>(t2) < 0 && t2 != ValueType::NaN)) {
                if (t1 == ValueType::Infinity || t1 == ValueType::NegativeInfinity || t2 == ValueType::Infinity || t2 == ValueType::NegativeInfinity) {
                    bool mode = (t1 == ValueType::Infinity || t2 == ValueType::Infinity);
                    const Value &value =
                        ((t1 == ValueType::Infinity && (t2 == ValueType::Infinity || t2 == ValueType::NegativeInfinity)) || (t2 != ValueType::Infinity && t2 != ValueType::NegativeInfinity)) ? v2 : v1;
                    if (value > 0)
                        return Value(mode ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
                    else if (value < 0)
                        return Value(mode ? SpecialValue::NegativeInfinity : SpecialValue::Infinity);
                    else
                        return Value(SpecialValue::NaN);
                }
            }

            if (v1.m_data.type == ValueType::Integer && v2.m_data.type == ValueType::Integer)
                return v1.m_data.intValue * v2.m_data.intValue;
            else
                return v1.toDouble() * v2.toDouble();
        }

        friend Value operator/(const Value &v1, const Value &v2)
        {
            if ((v1 == 0) && (v2 == 0))
                return Value(SpecialValue::NaN);
            else if (v2.toDouble() == 0)
                return v1 > 0 ? Value(SpecialValue::Infinity) : Value(SpecialValue::NegativeInfinity);
            else if ((v1.m_data.type == ValueType::Infinity || v1.m_data.type == ValueType::NegativeInfinity) &&
                     (v2.m_data.type == ValueType::Infinity || v2.m_data.type == ValueType::NegativeInfinity)) {
                return Value(SpecialValue::NaN);
            } else if (v1.m_data.type == ValueType::Infinity || v1.m_data.type == ValueType::NegativeInfinity) {
                if (v2.toDouble() < 0)
                    return v1.m_data.type == ValueType::Infinity ? Value(SpecialValue::NegativeInfinity) : Value(SpecialValue::Infinity);
                else
                    return v1.m_data.type == ValueType::Infinity ? Value(SpecialValue::Infinity) : Value(SpecialValue::NegativeInfinity);
            } else if (v2.m_data.type == ValueType::Infinity || v2.m_data.type == ValueType::NegativeInfinity) {
                return 0;
            }
            return v1.toDouble() / v2.toDouble();
        }

        friend Value operator%(const Value &v1, const Value &v2)
        {

            if ((v2 == 0) || (v1.m_data.type == ValueType::Infinity || v1.m_data.type == ValueType::NegativeInfinity))
                return Value(SpecialValue::NaN);
            else if (v2.m_data.type == ValueType::Infinity || v2.m_data.type == ValueType::NegativeInfinity) {
                return v1.toDouble();
            }
            if (v1 < 0 || v2 < 0)
                return fmod(v2.toDouble() + fmod(v1.toDouble(), -v2.toDouble()), v2.toDouble());
            else
                return fmod(v1.toDouble(), v2.toDouble());
        }

        static bool stringsEqual(std::u16string s1, std::u16string s2)
        {
            std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
            std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
            return (s1.compare(s2) == 0);
        }

        static bool stringsEqual(std::string s1, std::string s2)
        {
            std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
            std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
            return (s1.compare(s2) == 0);
        }

        static long hexToDec(const std::string &s)
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

        static long octToDec(const std::string &s)
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

        static double binToDec(const std::string &s)
        {
            static const std::string digits = "01";

            for (char c : s) {
                if (digits.find(c) == std::string::npos) {
                    return 0;
                }
            }

            return std::stoi(s, 0, 2);
        }

        static double stringToDouble(const std::string &s, bool *ok = nullptr)
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
                    return hexToDec(sub);
                } else if (s[1] == 'o' || s[1] == 'O') {
                    return octToDec(sub);
                } else if (s[1] == 'b' || s[1] == 'B') {
                    return binToDec(sub);
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

        static long stringToLong(const std::string &s, bool *ok = nullptr)
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
                    return hexToDec(sub);
                } else if (s[1] == 'o' || s[1] == 'O') {
                    return octToDec(sub);
                } else if (s[1] == 'b' || s[1] == 'B') {
                    return binToDec(sub);
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

        static std::string doubleToString(double v)
        {
            std::stringstream stream;

            if (v != 0) {
                const int exponent = std::log10(std::abs(v));

                if (exponent > 20)
                    stream << std::scientific << std::setprecision(digitCount(v / std::pow(10, exponent + 1)) - 1) << v;
                else
                    stream << std::setprecision(std::max(16u, digitCount(v))) << v;
            } else
                stream << std::setprecision(std::max(16u, digitCount(v))) << v;

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

        static double floatToDouble(float v)
        {
            unsigned int digits = digitCount(v);
            double f = std::pow(10, digits);
            return std::round(v * f) / f;
        }

        template<typename T>
        static unsigned int digitCount(T v)
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
        static bool isInf(T v)
        {
            return v > 0 && std::isinf(v);
        }

        template<typename T>
        static bool isNegativeInf(T v)
        {
            return v < 0 && std::isinf(v);
        }
};

} // namespace libscratchcpp
