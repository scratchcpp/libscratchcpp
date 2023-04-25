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
                    default:
                        return false;
                }
            } else {
                if (v1.isNumber() || v2.isNumber())
                    return v1.toNumber() == v2.toNumber();
                else if (v1.isBool() || v2.isBool())
                    return v1.toBool() == v2.toBool();
                else if (v1.isString() || v2.isString())
                    return stringsEqual(v1.toUtf16(), v2.toUtf16());
                else
                    return false;
            }
        }
};

} // namespace libscratchcpp
