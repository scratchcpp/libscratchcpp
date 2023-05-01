// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include <string>
#include <variant>
#include <ctgmath>

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
            Number,
            Bool,
            String,
            Special
        };

        Value(float numberValue);
        Value(double numberValue);
        Value(int numberValue = 0);
        Value(size_t numberValue);
        Value(long numberValue);
        Value(bool boolValue);
        Value(const std::string &stringValue);
        Value(const char *stringValue);
        Value(SpecialValue specialValue);

        Type type() const;

        bool isInfinity() const;
        bool isNegativeInfinity() const;
        bool isNaN() const;
        bool isNumber() const;
        bool isBool() const;
        bool isString() const;

        int toInt() const;
        long toLong() const;
        double toDouble() const;
        bool toBool() const;
        std::string toString() const;
        std::u16string toUtf16() const;

        inline const Value &operator=(float v)
        {
            m_type = Type::Number;
            m_value = v;
            return *this;
        }

        inline const Value &operator=(double v)
        {
            m_type = Type::Number;
            m_value = v;
            return *this;
        }

        inline const Value &operator=(int v)
        {
            m_type = Type::Number;
            m_value = v;
            return *this;
        }

        inline const Value &operator=(long v)
        {
            m_type = Type::Number;
            m_value = v;
            return *this;
        }

        inline const Value &operator=(bool v)
        {
            m_type = Type::Bool;
            m_value = v;
            return *this;
        }

        inline const Value &operator=(const std::string &v)
        {
            m_type = Type::String;
            m_value = v;
            return *this;
        }

        inline const Value &operator=(const char *v)
        {
            m_type = Type::String;
            m_value = v;
            return *this;
        }

    private:
        static bool stringsEqual(std::u16string s1, std::u16string s2);
        static bool stringsEqual(std::string s1, std::string s2);
        static double stringToDouble(const std::string &s, bool *ok = nullptr);
        static long stringToLong(const std::string &s, bool *ok = nullptr);

        bool m_isInfinity = false;
        bool m_isNegativeInfinity = false;
        bool m_isNaN = false;
        Type m_type;
        std::variant<long, double, bool, std::string> m_value;

        friend inline bool operator==(const Value &v1, const Value &v2)
        {
            if (v1.m_type == v2.m_type) {
                auto type = v1.m_type;
                switch (type) {
                    case Type::Number:
                        return v1.toDouble() == v2.toDouble();
                    case Type::Bool:
                        return v1.toBool() == v2.toBool();
                    case Type::String:
                        return stringsEqual(v1.toUtf16(), v2.toUtf16());
                    case Type::Special:
                        if (v1.isNaN() || v2.isNaN())
                            return false;
                        else
                            return ((v1.m_isInfinity && v2.m_isInfinity) || (v1.m_isNegativeInfinity && v2.m_isNegativeInfinity));
                }
            } else {
                if (v1.isNumber() || v2.isNumber())
                    return v1.toDouble() == v2.toDouble();
                else if (v1.isBool() || v2.isBool())
                    return ((!v1.isNaN() && !v2.isNaN()) && (v1.toBool() == v2.toBool()));
                else if (v1.isString() || v2.isString())
                    return stringsEqual(v1.toUtf16(), v2.toUtf16());
                else
                    return false;
            }
            return false;
        }

        friend inline bool operator>(const Value &v1, const Value &v2)
        {
            if (v1.m_isInfinity) {
                return !v2.m_isInfinity;
            } else if (v1.m_isNegativeInfinity)
                return false;
            else if (v2.m_isInfinity)
                return false;
            else if (v2.m_isNegativeInfinity)
                return true;
            return v1.toDouble() > v2.toDouble();
        }

        friend inline bool operator<(const Value &v1, const Value &v2)
        {
            if (v1.m_isInfinity) {
                return false;
            } else if (v1.m_isNegativeInfinity)
                return !v2.m_isNegativeInfinity;
            else if (v2.m_isInfinity)
                return !v1.m_isInfinity;
            else if (v2.m_isNegativeInfinity)
                return false;
            return v1.toDouble() < v2.toDouble();
        }

        friend inline bool operator>=(const Value &v1, const Value &v2) { return v1 > v2 || v1 == v2; }

        friend inline bool operator<=(const Value &v1, const Value &v2) { return v1 < v2 || v1 == v2; }

        friend inline Value operator+(const Value &v1, const Value &v2)
        {
            if ((v1.m_isInfinity && v2.m_isNegativeInfinity) || (v1.m_isNegativeInfinity && v2.m_isInfinity))
                return Value(SpecialValue::NaN);
            else if (v1.m_isInfinity || v2.m_isInfinity)
                return Value(SpecialValue::Infinity);
            else if (v1.m_isNegativeInfinity || v2.m_isNegativeInfinity)
                return Value(SpecialValue::NegativeInfinity);
            return v1.toDouble() + v2.toDouble();
        }

        friend inline Value operator-(const Value &v1, const Value &v2)
        {
            if ((v1.m_isInfinity && v2.m_isInfinity) || (v1.m_isNegativeInfinity && v2.m_isNegativeInfinity))
                return Value(SpecialValue::NaN);
            else if (v1.m_isInfinity || v2.m_isNegativeInfinity)
                return Value(SpecialValue::Infinity);
            else if (v1.m_isNegativeInfinity || v2.m_isInfinity)
                return Value(SpecialValue::NegativeInfinity);
            return v1.toDouble() - v2.toDouble();
        }

        friend inline Value operator*(const Value &v1, const Value &v2)
        {
            if (v1.m_isInfinity || v1.m_isNegativeInfinity || v2.m_isInfinity || v2.m_isNegativeInfinity) {
                bool mode = (v1.m_isInfinity || v2.m_isInfinity);
                const Value &value = (v1.m_isInfinity || v1.m_isNegativeInfinity) ? v2 : v1;
                if (value > 0)
                    return Value(mode ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
                else if (value < 0)
                    return Value(mode ? SpecialValue::NegativeInfinity : SpecialValue::Infinity);
                else
                    return Value(SpecialValue::NaN);
            }
            return v1.toDouble() * v2.toDouble();
        }

        friend inline Value operator/(const Value &v1, const Value &v2)
        {

            if ((v1 == 0) && (v2 == 0))
                return Value(SpecialValue::NaN);
            else if (v2 == 0) {
                if (v2.m_isInfinity || v2.m_isNegativeInfinity) {
                    if (v1.m_isInfinity || v1.m_isNegativeInfinity)
                        return Value(SpecialValue::NaN);
                    else
                        return 0;
                } else
                    return Value(v1 > 0 ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
            }
            return v1.toDouble() / v2.toDouble();
        }

        friend inline Value operator%(const Value &v1, const Value &v2)
        {

            if ((v2 == 0) || (v1.m_isInfinity || v1.m_isNegativeInfinity))
                return Value(SpecialValue::NaN);
            else if (v2.m_isInfinity || v2.m_isNegativeInfinity) {
                return v1.toDouble();
            }
            return fmod(v1.toDouble(), v2.toDouble());
        }
};

} // namespace libscratchcpp
