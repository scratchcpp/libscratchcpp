// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <variant>
#include <algorithm>
#include <limits>
#include <ctgmath>
#include <utf8.h>

#include "global.h"

namespace libscratchcpp
{

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

        enum class Type
        {
            Integer = 0,
            Double = 1,
            Bool = 2,
            String = 3,
            Infinity = -1,
            NegativeInfinity = -2,
            NaN = -3
        };

        /*! Constructs a number Value. */
        Value(float numberValue) :
            m_type(Type::Double)
        {
            m_doubleValue = numberValue;
        }

        /*! Constructs a number Value. */
        Value(double numberValue) :
            m_type(Type::Double)
        {
            m_doubleValue = numberValue;
        }

        /*! Constructs a number Value. */
        Value(int numberValue = 0) :
            m_type(Type::Integer)
        {
            m_intValue = numberValue;
        }

        /*! Constructs a number Value. */
        Value(size_t numberValue) :
            m_type(Type::Integer)
        {
            m_intValue = numberValue;
        }

        /*! Constructs a number Value. */
        Value(long numberValue) :
            m_type(Type::Integer)
        {
            m_intValue = numberValue;
        }

        /*! Constructs a boolean Value. */
        Value(bool boolValue) :
            m_type(Type::Bool)
        {
            m_boolValue = boolValue;
        }

        /*! Constructs a string Value. */
        Value(const std::string &stringValue) :
            m_type(Type::String)
        {
            new (&m_stringValue) std::string(stringValue);
            initString(stringValue);
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
                m_type = Type::Infinity;
            else if (specialValue == SpecialValue::NegativeInfinity)
                m_type = Type::NegativeInfinity;
            else if (specialValue == SpecialValue::NaN)
                m_type = Type::NaN;
            else {
                m_type = Type::Integer;
                m_intValue = 0;
            }
        }

        Value(const Value &v)
        {
            m_type = v.m_type;

            switch (m_type) {
                case Type::Integer:
                    m_intValue = v.m_intValue;
                    break;
                case Type::Double:
                    m_doubleValue = v.m_doubleValue;
                    break;
                case Type::Bool:
                    m_boolValue = v.m_boolValue;
                    break;
                case Type::String:
                    new (&m_stringValue) std::string(v.m_stringValue);
                    break;
                default:
                    break;
            }
        }

        ~Value()
        {
            if (m_type == Type::String)
                m_stringValue.~basic_string();
        }

        /*! Returns the type of the value. */
        Type type() const { return m_type; }

        /*! Returns true if the value is infinity. */
        bool isInfinity() const { return m_type == Type::Infinity; }

        /*! Returns true if the value is negative infinity. */
        bool isNegativeInfinity() const { return m_type == Type::NegativeInfinity; }

        /*! Returns true if the value is NaN (Not a Number). */
        bool isNaN() const { return m_type == Type::NaN; }

        /*! Returns true if the value is a number. */
        bool isNumber() const { return m_type == Type::Integer || m_type == Type::Double; }

        /*! Returns true if the value is a boolean. */
        bool isBool() const { return m_type == Type::Bool; }

        /*! Returns true if the value is a string. */
        bool isString() const { return m_type == Type::String; }

        /*! Returns the int representation of the value. */
        int toInt() const { return toLong(); }

        /*! Returns the long representation of the value. */
        long toLong() const
        {
            if (static_cast<int>(m_type) < 0) {
                switch (m_type) {
                    case Type::Infinity:
                        return std::numeric_limits<long>::infinity();
                    case Type::NegativeInfinity:
                        return -std::numeric_limits<long>::infinity();
                    default:
                        return 0;
                }
            } else {
                switch (m_type) {
                    case Type::Integer:
                        return m_intValue;
                    case Type::Double:
                        return m_doubleValue;
                    case Type::Bool:
                        return m_boolValue;
                    case Type::String:
                        return stringToLong(m_stringValue);
                    default:
                        return 0;
                }
            }
        }

        /*! Returns the double representation of the value. */
        double toDouble() const
        {
            if (static_cast<int>(m_type) < 0) {
                switch (m_type) {
                    case Type::Infinity:
                        return std::numeric_limits<double>::infinity();
                    case Type::NegativeInfinity:
                        return -std::numeric_limits<double>::infinity();
                    default:
                        return 0;
                }
            } else {
                switch (m_type) {
                    case Type::Double:
                        return m_doubleValue;
                    case Type::Integer:
                        return m_intValue;
                    case Type::Bool:
                        return m_boolValue;
                    case Type::String:
                        return stringToDouble(m_stringValue);
                    default:
                        return 0;
                }
            }
        };

        /*! Returns the boolean representation of the value. */
        bool toBool() const
        {
            if (static_cast<int>(m_type) < 0) {
                return false;
            } else {
                switch (m_type) {
                    case Type::Bool:
                        return m_boolValue;
                    case Type::Integer:
                        return m_intValue == 1 ? true : false;
                    case Type::Double:
                        return m_doubleValue == 1 ? true : false;
                    case Type::String: {
                        const std::string &str = m_stringValue;
                        return (stringsEqual(str, "true") || str == "1");
                    }
                    default:
                        return false;
                }
            }
        };

        /*! Returns the string representation of the value. */
        std::string toString() const
        {
            if (static_cast<int>(m_type) < 0) {
                switch (m_type) {
                    case Type::Infinity:
                        return "Infinity";
                    case Type::NegativeInfinity:
                        return "-Infinity";
                    default:
                        return "NaN";
                }
            } else {
                switch (m_type) {
                    case Type::String:
                        return m_stringValue;
                    case Type::Integer:
                        return std::to_string(m_intValue);
                    case Type::Double: {
                        std::stringstream stream;
                        stream << m_doubleValue;
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
                    case Type::Bool:
                        return m_boolValue ? "true" : "false";
                    default:
                        return "";
                }
            }
        };

        /*! Returns the UTF-16 representation of the value. */
        std::u16string toUtf16() const { return utf8::utf8to16(toString()); };

        /*! Adds the given value to the value. */
        void add(const Value &v)
        {
            if ((static_cast<int>(m_type) < 0) || (static_cast<int>(v.m_type) < 0)) {
                if ((m_type == Type::Infinity && v.m_type == Type::NegativeInfinity) || (m_type == Type::NegativeInfinity && v.m_type == Type::Infinity))
                    m_type = Type::NaN;
                else if (m_type == Type::Infinity || v.m_type == Type::Infinity)
                    m_type = Type::Infinity;
                else if (m_type == Type::NegativeInfinity || v.m_type == Type::NegativeInfinity)
                    m_type = Type::NegativeInfinity;
                return;
            }

            if (m_type == Type::Integer && v.m_type == Type::Integer)
                m_intValue += v.m_intValue;
            else
                *this = toDouble() + v.toDouble();
        }

        /*! Subtracts the given value from the value. */
        void subtract(const Value &v)
        {
            if ((static_cast<int>(m_type) < 0) || (static_cast<int>(v.m_type) < 0)) {
                if ((m_type == Type::Infinity && v.m_type == Type::Infinity) || (m_type == Type::NegativeInfinity && v.m_type == Type::NegativeInfinity))
                    m_type = Type::NaN;
                else if (m_type == Type::Infinity || v.m_type == Type::NegativeInfinity)
                    m_type = Type::Infinity;
                else if (m_type == Type::NegativeInfinity || v.m_type == Type::Infinity)
                    m_type = Type::NegativeInfinity;
                return;
            }

            if (m_type == Type::Integer && v.m_type == Type::Integer)
                m_intValue -= v.m_intValue;
            else
                *this = toDouble() - v.toDouble();
        }

        /*! Multiplies the given value with the value. */
        void multiply(const Value &v)
        {
            Type t1 = m_type, t2 = v.m_type;
            if ((static_cast<int>(t1) < 0 && t1 != Type::NaN) || (static_cast<int>(t2) < 0 && t2 != Type::NaN)) {
                if (t1 == Type::Infinity || t1 == Type::NegativeInfinity || t2 == Type::Infinity || t2 == Type::NegativeInfinity) {
                    bool mode = (t1 == Type::Infinity || t2 == Type::Infinity);
                    const Value &value = ((t1 == Type::Infinity && (t2 == Type::Infinity || t2 == Type::NegativeInfinity)) || (t2 != Type::Infinity && t2 != Type::NegativeInfinity)) ? v : *this;
                    if (value > 0)
                        m_type = mode ? Type::Infinity : Type::NegativeInfinity;
                    else if (value < 0)
                        m_type = mode ? Type::NegativeInfinity : Type::Infinity;
                    else
                        m_type = Type::NaN;
                    return;
                }
            }

            if (m_type == Type::Integer && v.m_type == Type::Integer)
                m_intValue *= v.m_intValue;
            else
                *this = toDouble() * v.toDouble();
        }

        /*! Divides the value by the given value. */
        void divide(const Value &v)
        {
            if ((toDouble() == 0) && (v.toDouble() == 0)) {
                m_type = Type::NaN;
                return;
            } else if (v.toDouble() == 0) {
                m_type = *this > 0 ? Type::Infinity : Type::NegativeInfinity;
                return;
            } else if ((m_type == Type::Infinity || m_type == Type::NegativeInfinity) && (v.m_type == Type::Infinity || v.m_type == Type::NegativeInfinity)) {
                m_type = Type::NaN;
                return;
            } else if (m_type == Type::Infinity || m_type == Type::NegativeInfinity) {
                if (v.toDouble() < 0)
                    m_type = m_type == Type::Infinity ? Type::NegativeInfinity : Type::Infinity;
                return;
            } else if (v.m_type == Type::Infinity || v.m_type == Type::NegativeInfinity) {
                *this = 0;
                return;
            }
            *this = toDouble() / v.toDouble();
        }

        /*! Replaces the value with modulo of the value and the given value. */
        void mod(const Value &v)
        {
            if ((v == 0) || (m_type == Type::Infinity || m_type == Type::NegativeInfinity)) {
                m_type = Type::NaN;
                return;
            } else if (v.m_type == Type::Infinity || v.m_type == Type::NegativeInfinity) {
                return;
            }
            if (*this < 0 || v < 0)
                *this = fmod(v.toDouble() + fmod(toDouble(), -v.toDouble()), v.toDouble());
            else
                *this = fmod(toDouble(), v.toDouble());
        }

        const Value &operator=(float v)
        {
            m_type = Type::Double;
            m_doubleValue = v;
            return *this;
        }

        const Value &operator=(double v)
        {
            m_type = Type::Double;
            m_doubleValue = v;
            return *this;
        }

        const Value &operator=(int v)
        {
            m_type = Type::Integer;
            m_intValue = v;
            return *this;
        }

        const Value &operator=(long v)
        {
            m_type = Type::Integer;
            m_intValue = v;
            return *this;
        }

        const Value &operator=(bool v)
        {
            m_type = Type::Bool;
            m_boolValue = v;
            return *this;
        }

        const Value &operator=(const std::string &v)
        {
            m_type = Type::String;
            new (&m_stringValue) std::string(v);
            initString(v);
            return *this;
        }

        const Value &operator=(const char *v)
        {
            m_type = Type::String;
            new (&m_stringValue) std::string(v);
            initString(v);
            return *this;
        }

        const Value &operator=(const Value &v)
        {
            m_type = v.m_type;

            switch (m_type) {
                case Type::Integer:
                    m_intValue = v.m_intValue;
                    break;
                case Type::Double:
                    m_doubleValue = v.m_doubleValue;
                    break;
                case Type::Bool:
                    m_boolValue = v.m_boolValue;
                    break;
                case Type::String:
                    new (&m_stringValue) std::string(v.m_stringValue);
                    break;
                default:
                    break;
            }

            return *this;
        }

    private:
        union
        {
                long m_intValue;
                double m_doubleValue;
                bool m_boolValue;
                std::string m_stringValue;
        };

        void initString(const std::string &str)
        {
            if (str.empty())
                return;
            else if (str == "Infinity") {
                m_type = Type::Infinity;
                return;
            } else if (str == "-Infinity") {
                m_type = Type::NegativeInfinity;
                return;
            } else if (str == "NaN") {
                m_type = Type::NaN;
                return;
            }

            bool ok;
            bool isLong = false;
            long l;
            double d;
            if ((str.find_first_of('.') == std::string::npos) && (str.find_first_of('e') == std::string::npos) && (str.find_first_of('E') == std::string::npos)) {
                l = stringToLong(str, &ok);
                isLong = true;
            } else
                d = stringToDouble(str, &ok);
            if (ok) {
                if (isLong) {
                    *this = l;
                    m_type = Type::Integer;
                } else {
                    *this = d;
                    m_type = Type::Double;
                }
            }
        }

        void initString(const char *str) { initString(std::string(str)); }

        Type m_type;

        friend bool operator==(const Value &v1, const Value &v2)
        {
            if (v1.m_type == v2.m_type) {
                auto type = v1.m_type;
                switch (type) {
                    case Type::Integer:
                        return v1.toLong() == v2.toLong();
                    case Type::Double:
                        return v1.toDouble() == v2.toDouble();
                    case Type::Bool:
                        return v1.toBool() == v2.toBool();
                    case Type::String:
                        return stringsEqual(v1.toUtf16(), v2.toUtf16());
                    default:
                        if ((static_cast<int>(v1.m_type) < 0) && (static_cast<int>(v2.m_type) < 0)) {
                            return v1.m_type == v2.m_type;
                        }
                }
            } else {
                if (v1.isString() || v2.isString())
                    return stringsEqual(v1.toUtf16(), v2.toUtf16());
                else if (v1.isNumber() || v2.isNumber())
                    return v1.toDouble() == v2.toDouble();
                else if (v1.isBool() || v2.isBool())
                    return ((v1.m_type != Type::NaN && v2.m_type != Type::NaN) && (v1.toBool() == v2.toBool()));
                else
                    return false;
            }
            return false;
        }

        friend bool operator!=(const Value &v1, const Value &v2) { return !(v1 == v2); }

        friend bool operator>(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if (v1.m_type == Type::Infinity) {
                    return v2.m_type != Type::Infinity;
                } else if (v1.m_type == Type::NegativeInfinity)
                    return false;
                else if (v2.m_type == Type::Infinity)
                    return false;
                else if (v2.m_type == Type::NegativeInfinity)
                    return true;
            }

            if (v1.m_type == Type::Integer && v2.m_type == Type::Integer)
                return v1.m_intValue > v2.m_intValue;
            else
                return v1.toDouble() > v2.toDouble();
        }

        friend bool operator<(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if (v1.m_type == Type::Infinity) {
                    return false;
                } else if (v1.m_type == Type::NegativeInfinity)
                    return v2.m_type != Type::NegativeInfinity;
                else if (v2.m_type == Type::Infinity)
                    return v1.m_type != Type::Infinity;
                else if (v2.m_type == Type::NegativeInfinity)
                    return false;
            }

            if (v1.m_type == Type::Integer && v2.m_type == Type::Integer)
                return v1.m_intValue < v2.m_intValue;
            else
                return v1.toDouble() < v2.toDouble();
        }

        friend bool operator>=(const Value &v1, const Value &v2) { return v1 > v2 || v1 == v2; }

        friend bool operator<=(const Value &v1, const Value &v2) { return v1 < v2 || v1 == v2; }

        friend Value operator+(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if ((v1.m_type == Type::Infinity && v2.m_type == Type::NegativeInfinity) || (v1.m_type == Type::NegativeInfinity && v2.m_type == Type::Infinity))
                    return Value(SpecialValue::NaN);
                else if (v1.m_type == Type::Infinity || v2.m_type == Type::Infinity)
                    return Value(SpecialValue::Infinity);
                else if (v1.m_type == Type::NegativeInfinity || v2.m_type == Type::NegativeInfinity)
                    return Value(SpecialValue::NegativeInfinity);
            }

            if (v1.m_type == Type::Integer && v2.m_type == Type::Integer)
                return v1.m_intValue + v2.m_intValue;
            else
                return v1.toDouble() + v2.toDouble();
        }

        friend Value operator-(const Value &v1, const Value &v2)
        {
            if ((static_cast<int>(v1.m_type) < 0) || (static_cast<int>(v2.m_type) < 0)) {
                if ((v1.m_type == Type::Infinity && v2.m_type == Type::Infinity) || (v1.m_type == Type::NegativeInfinity && v2.m_type == Type::NegativeInfinity))
                    return Value(SpecialValue::NaN);
                else if (v1.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity)
                    return Value(SpecialValue::Infinity);
                else if (v1.m_type == Type::NegativeInfinity || v2.m_type == Type::Infinity)
                    return Value(SpecialValue::NegativeInfinity);
            }

            if (v1.m_type == Type::Integer && v2.m_type == Type::Integer)
                return v1.m_intValue - v2.m_intValue;
            else
                return v1.toDouble() - v2.toDouble();
        }

        friend Value operator*(const Value &v1, const Value &v2)
        {
            Type t1 = v1.m_type, t2 = v2.m_type;
            if ((static_cast<int>(t1) < 0 && t1 != Type::NaN) || (static_cast<int>(t2) < 0 && t2 != Type::NaN)) {
                if (t1 == Type::Infinity || t1 == Type::NegativeInfinity || t2 == Type::Infinity || t2 == Type::NegativeInfinity) {
                    bool mode = (t1 == Type::Infinity || t2 == Type::Infinity);
                    const Value &value = ((t1 == Type::Infinity && (t2 == Type::Infinity || t2 == Type::NegativeInfinity)) || (t2 != Type::Infinity && t2 != Type::NegativeInfinity)) ? v2 : v1;
                    if (value > 0)
                        return Value(mode ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
                    else if (value < 0)
                        return Value(mode ? SpecialValue::NegativeInfinity : SpecialValue::Infinity);
                    else
                        return Value(SpecialValue::NaN);
                }
            }

            if (v1.m_type == Type::Integer && v2.m_type == Type::Integer)
                return v1.m_intValue * v2.m_intValue;
            else
                return v1.toDouble() * v2.toDouble();
        }

        friend Value operator/(const Value &v1, const Value &v2)
        {
            if ((v1 == 0) && (v2 == 0))
                return Value(SpecialValue::NaN);
            else if (v2.toDouble() == 0)
                return v1 > 0 ? Value(SpecialValue::Infinity) : Value(SpecialValue::NegativeInfinity);
            else if ((v1.m_type == Type::Infinity || v1.m_type == Type::NegativeInfinity) && (v2.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity)) {
                return Value(SpecialValue::NaN);
            } else if (v1.m_type == Type::Infinity || v1.m_type == Type::NegativeInfinity) {
                if (v2.toDouble() < 0)
                    return v1.m_type == Type::Infinity ? Value(SpecialValue::NegativeInfinity) : Value(SpecialValue::Infinity);
                else
                    return v1.m_type == Type::Infinity ? Value(SpecialValue::Infinity) : Value(SpecialValue::NegativeInfinity);
            } else if (v2.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity) {
                return 0;
            }
            return v1.toDouble() / v2.toDouble();
        }

        friend Value operator%(const Value &v1, const Value &v2)
        {

            if ((v2 == 0) || (v1.m_type == Type::Infinity || v1.m_type == Type::NegativeInfinity))
                return Value(SpecialValue::NaN);
            else if (v2.m_type == Type::Infinity || v2.m_type == Type::NegativeInfinity) {
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

            static const std::string digits = "0123456789.eE+-";
            for (char c : s) {
                if (digits.find(c) == std::string::npos) {
                    return 0;
                }
            }
            try {
                if (ok)
                    *ok = true;
                return std::stod(s);
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

            static const std::string digits = "0123456789+-";

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
};

} // namespace libscratchcpp
