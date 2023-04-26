// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include <string>

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

        Value(float numberValue = 0);
        Value(double numberValue);
        Value(int numberValue);
        Value(uint32_t numberValue);
        Value(bool boolValue);
        Value(std::string stringValue);
        Value(const char *stringValue);
        Value(SpecialValue specialValue);

        Type type() const;

        bool isInfinity() const;
        bool isNegativeInfinity() const;
        bool isNaN() const;
        bool isNumber() const;
        bool isBool() const;
        bool isString() const;

        float toNumber() const;
        bool toBool() const;
        std::string toString() const;
        std::u16string toUtf16() const;

    private:
        static bool stringsEqual(std::u16string s1, std::u16string s2);
        static bool stringsEqual(std::string s1, std::string s2);
        static float stringToFloat(std::string s, bool *ok = nullptr);

        bool m_isInfinity = false;
        bool m_isNegativeInfinity = false;
        bool m_isNaN = false;
        Type m_type;
        float m_numberValue = 0;
        bool m_boolValue = false;
        std::string m_stringValue;

        friend inline bool operator==(const Value &v1, const Value &v2)
        {
            if (v1.m_type == v2.m_type) {
                auto type = v1.m_type;
                switch (type) {
                    case Type::Number:
                        return v1.m_numberValue == v2.m_numberValue;
                    case Type::Bool:
                        return v1.m_boolValue == v2.m_boolValue;
                    case Type::String:
                        return stringsEqual(v1.toUtf16(), v2.toUtf16());
                    case Type::Special:
                        if (v1.isNaN() || v2.isNaN())
                            return false;
                        else
                            return ((v1.isInfinity() && v2.isInfinity()) || (v1.isNegativeInfinity() && v2.isNegativeInfinity()));
                }
            } else {
                if (v1.isNumber() || v2.isNumber())
                    return v1.toNumber() == v2.toNumber();
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
            if (v1.isInfinity()) {
                return !v2.isInfinity();
            } else if (v1.isNegativeInfinity())
                return false;
            else if (v2.isInfinity())
                return false;
            else if (v2.isNegativeInfinity())
                return true;
            return v1.toNumber() > v2.toNumber();
        }

        friend inline bool operator<(const Value &v1, const Value &v2)
        {
            if (v1.isInfinity()) {
                return false;
            } else if (v1.isNegativeInfinity())
                return !v2.isNegativeInfinity();
            else if (v2.isInfinity())
                return !v1.isInfinity();
            else if (v2.isNegativeInfinity())
                return false;
            return v1.toNumber() < v2.toNumber();
        }

        friend inline bool operator>=(const Value &v1, const Value &v2) { return v1 > v2 || v1 == v2; }

        friend inline bool operator<=(const Value &v1, const Value &v2) { return v1 < v2 || v1 == v2; }

        friend inline Value operator+(const Value &v1, const Value &v2)
        {
            if ((v1.isInfinity() && v2.isNegativeInfinity()) || (v1.isNegativeInfinity() && v2.isInfinity()))
                return Value(SpecialValue::NaN);
            else if (v1.isInfinity() || v2.isInfinity())
                return Value(SpecialValue::Infinity);
            else if (v1.isNegativeInfinity() || v2.isNegativeInfinity())
                return Value(SpecialValue::NegativeInfinity);
            return v1.toNumber() + v2.toNumber();
        }

        friend inline Value operator-(const Value &v1, const Value &v2)
        {
            if ((v1.isInfinity() && v2.isInfinity()) || (v1.isNegativeInfinity() && v2.isNegativeInfinity()))
                return Value(SpecialValue::NaN);
            else if (v1.isInfinity() || v2.isNegativeInfinity())
                return Value(SpecialValue::Infinity);
            else if (v1.isNegativeInfinity() || v2.isInfinity())
                return Value(SpecialValue::NegativeInfinity);
            return v1.toNumber() - v2.toNumber();
        }

        friend inline Value operator*(const Value &v1, const Value &v2)
        {
            if (v1.isInfinity() || v1.isNegativeInfinity() || v2.isInfinity() || v2.isNegativeInfinity()) {
                bool mode = (v1.isInfinity() || v2.isInfinity());
                const Value &value = (v1.isInfinity() || v1.isNegativeInfinity()) ? v2 : v1;
                if (value > 0)
                    return Value(mode ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
                else if (value < 0)
                    return Value(mode ? SpecialValue::NegativeInfinity : SpecialValue::Infinity);
                else
                    return Value(SpecialValue::NaN);
            }
            return v1.toNumber() * v2.toNumber();
        }

        friend inline Value operator/(const Value &v1, const Value &v2)
        {

            if ((v1 == 0) && (v2 == 0))
                return Value(SpecialValue::NaN);
            else if (v2 == 0) {
                if (v2.isInfinity() || v2.isNegativeInfinity()) {
                    if (v1.isInfinity() || v1.isNegativeInfinity())
                        return Value(SpecialValue::NaN);
                    else
                        return 0;
                } else
                    return Value(v1 > 0 ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
            }
            return v1.toNumber() / v2.toNumber();
        }

        friend inline Value operator%(const Value &v1, const Value &v2)
        {

            if ((v2 == 0) || (v1.isInfinity() || v1.isNegativeInfinity()))
                return Value(SpecialValue::NaN);
            else if (v2.isInfinity() || v2.isNegativeInfinity()) {
                return v1.toNumber();
            }
            return static_cast<int>(v1.toNumber()) % static_cast<int>(v2.toNumber());
        }
};

} // namespace libscratchcpp
