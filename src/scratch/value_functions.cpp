#include <ctgmath>
#include <cassert>
#include <utf8.h>

#include "value_functions_p.h"

namespace libscratchcpp
{

extern "C"
{
    /* free */

    /*! Frees memory used by the given value if it's a string. */
    void value_free(ValueData *v)
    {
        if (v->type == ValueType::String) {
            assert(v->stringValue);
            free(v->stringValue);
            v->stringValue = nullptr;
            v->stringSize = 0;
        }
    }

    /* init */

    /*! Initializes the given value. */
    void value_init(ValueData *v)
    {
        v->type = ValueType::Number;
        v->numberValue = 0;
    }

    /* assign */

    /*! Assigns number of type 'double' to the given value. */
    void value_assign_double(ValueData *v, double numberValue)
    {
        value_free(v);

        v->type = ValueType::Number;
        v->numberValue = numberValue;
    }

    /*! Assigns boolean to the given value. */
    void value_assign_bool(ValueData *v, bool boolValue)
    {
        value_free(v);

        v->type = ValueType::Bool;
        v->boolValue = boolValue;
    }

    /*! Assigns string to the given value. */
    void value_assign_string(ValueData *v, const std::string &stringValue)
    {
        value_assign_cstring(v, stringValue.c_str());
    }

    /*! Assigns C string to the given value. */
    void value_assign_cstring(ValueData *v, const char *stringValue)
    {
        if (v->type == ValueType::String) {
            value_replaceStr(v, stringValue);
        } else {
            value_free(v);
            value_initStr(v, stringValue);
        }
    }

    /*! Assigns special value to the given value. */
    void value_assign_special(ValueData *v, SpecialValue specialValue)
    {
        value_free(v);

        if (specialValue == SpecialValue::Infinity)
            v->type = ValueType::Infinity;
        else if (specialValue == SpecialValue::NegativeInfinity)
            v->type = ValueType::NegativeInfinity;
        else if (specialValue == SpecialValue::NaN)
            v->type = ValueType::NaN;
        else {
            v->type = ValueType::Number;
            v->numberValue = 0;
        }
    }

    /*! Assigns another value to the given value. */
    void value_assign_copy(ValueData *v, const libscratchcpp::ValueData *another)
    {
        if (another->type == ValueType::Number) {
            value_free(v);
            v->numberValue = another->numberValue;
        } else if (another->type == ValueType::Bool) {
            value_free(v);
            v->boolValue = another->boolValue;
        } else if (another->type == ValueType::String) {
            if (v->type == ValueType::String)
                value_replaceStr(v, another->stringValue);
            else {
                value_free(v);
                value_initStr(v, another->stringValue);
            }
        }

        v->type = another->type;
    }

    /* type checkers */

    /*! Returns true if the given value is Infinity. */
    bool value_isInfinity(const libscratchcpp::ValueData *v)
    {
        switch (v->type) {
            case ValueType::Infinity:
                return true;
            case ValueType::Number:
                return value_isInf(v->numberValue);
            case ValueType::String:
                return strcmp(v->stringValue, "Infinity") == 0;
            default:
                return false;
        }
    }

    /*! Returns true if the given value is -Infinity. */
    bool value_isNegativeInfinity(const libscratchcpp::ValueData *v)
    {
        switch (v->type) {
            case ValueType::NegativeInfinity:
                return true;
            case ValueType::Number:
                return value_isNegativeInf(v->numberValue);
            case ValueType::String:
                return strcmp(v->stringValue, "-Infinity") == 0;
            default:
                return false;
        }
    }

    /*! Returns true if the given value is NaN. */
    bool value_isNaN(const libscratchcpp::ValueData *v)
    {
        switch (v->type) {
            case ValueType::NaN:
                return true;
            case ValueType::Number:
                return std::isnan(v->numberValue);
            case ValueType::String:
                return strcmp(v->stringValue, "NaN") == 0;
            default:
                return false;
        }
    }

    /*! Returns true if the given value is a number. */
    bool value_isNumber(const libscratchcpp::ValueData *v)
    {
        return v->type == ValueType::Number;
    }

    /*! Returns true if the given value is a number or can be converted to a number. */
    bool value_isValidNumber(const libscratchcpp::ValueData *v)
    {
        if (value_isNaN(v))
            return false;

        if (value_isInfinity(v) || value_isNegativeInfinity(v))
            return true;

        assert(v->type != ValueType::Infinity && v->type != ValueType::NegativeInfinity);

        switch (v->type) {
            case ValueType::Number:
            case ValueType::Bool:
                return true;
            case ValueType::String:
                return strlen(v->stringValue) == 0 || value_checkString(v->stringValue) > 0;
            default:
                return false;
        }
    }

    /*! Returns true if the given value represents a round integer. */
    bool value_isInt(const ValueData *v)
    {
        // https://github.com/scratchfoundation/scratch-vm/blob/112989da0e7306eeb405a5c52616e41c2164af24/src/util/cast.js#L157-L181
        switch (v->type) {
            case ValueType::Bool:
            case ValueType::Infinity:
            case ValueType::NegativeInfinity:
            case ValueType::NaN:
                return true;
            case ValueType::Number: {
                double intpart;
                std::modf(v->numberValue, &intpart);
                return v->numberValue == intpart;
            }
            case ValueType::String:
                return value_checkString(v->stringValue) == 1;
        }

        return false;
    }

    /*! Returns true if the given value is a boolean. */
    bool value_isBool(const libscratchcpp::ValueData *v)
    {
        return v->type == ValueType::Bool;
    }

    /*! Returns true if the given value is a string. */
    bool value_isString(const libscratchcpp::ValueData *v)
    {
        return v->type == ValueType::String;
    }

    /* conversion */

    /*! Returns the long representation of the given value. */
    long value_toLong(const libscratchcpp::ValueData *v)
    {
        if (v->type == ValueType::Number)
            return v->numberValue;
        else if (v->type == ValueType::Bool)
            return v->boolValue;
        else if (v->type == ValueType::String)
            return value_stringToLong(v->stringValue);
        else
            return 0;
    }

    /*! Returns the int representation of the given value. */
    int value_toInt(const libscratchcpp::ValueData *v)
    {
        if (v->type == ValueType::Number)
            return v->numberValue;
        else if (v->type == ValueType::Bool)
            return v->boolValue;
        else if (v->type == ValueType::String)
            return value_stringToLong(v->stringValue);
        else
            return 0;
    }

    /*! Returns the double representation of the given value. */
    double value_toDouble(const libscratchcpp::ValueData *v)
    {
        if (v->type == ValueType::Number)
            return v->numberValue;
        else if (v->type == ValueType::Bool)
            return v->boolValue;
        else if (v->type == ValueType::String)
            return value_stringToDouble(v->stringValue);
        else if (v->type == ValueType::Infinity)
            return std::numeric_limits<double>::infinity();
        else if (v->type == ValueType::NegativeInfinity)
            return -std::numeric_limits<double>::infinity();
        else
            return 0;
    }

    /*! Returns the boolean representation of the given value. */
    bool value_toBool(const libscratchcpp::ValueData *v)
    {
        if (v->type == ValueType::Bool) {
            return v->boolValue;
        } else if (v->type == ValueType::Number) {
            return v->numberValue != 0;
        } else if (v->type == ValueType::String) {
            return value_stringToBool(v->stringValue);
        } else if (v->type == ValueType::Infinity || v->type == ValueType::NegativeInfinity) {
            return true;
        } else if (v->type == ValueType::NaN) {
            return false;
        } else {
            return false;
        }
    }

    /*! Writes the string representation of the given value to dst. */
    void value_toString(const libscratchcpp::ValueData *v, std::string *dst)
    {
        if (v->type == ValueType::String)
            dst->assign(v->stringValue);
        else if (v->type == ValueType::Number)
            value_doubleToString(v->numberValue, dst);
        else if (v->type == ValueType::Bool)
            dst->assign(v->boolValue ? "true" : "false");
        else if (v->type == ValueType::Infinity)
            dst->assign("Infinity");
        else if (v->type == ValueType::NegativeInfinity)
            dst->assign("-Infinity");
        else if (v->type == ValueType::NaN)
            dst->assign("NaN");
        else
            dst->clear();
    }

    /*!
     * Returns the string representation of the given value.
     * \note It is the caller's responsibility to free allocated memory.
     */
    char *value_toCString(const ValueData *v)
    {
        std::string out;
        value_toString(v, &out);
        char *ret = (char *)malloc((out.size() + 1) * sizeof(char));
        strncpy(ret, out.c_str(), out.size() + 1);
        return ret;
    }

    /*! Writes the UTF-16 representation of the given value to dst. */
    void value_toUtf16(const libscratchcpp::ValueData *v, std::u16string *dst)
    {
        std::string s;
        value_toString(v, &s);
        dst->assign(utf8::utf8to16(s));
    }

    /*!
     * Converts the given number to string.
     * \note It is the caller's responsibility to free allocated memory.
     */
    char *value_doubleToCString(double v)
    {
        std::string out;
        value_doubleToString(v, &out);
        char *ret = (char *)malloc((out.size() + 1) * sizeof(char));
        strncpy(ret, out.c_str(), out.size() + 1);
        return ret;
    }

    /*!
     * Converts the given boolean to string.
     * \note Do not free allocated memory!
     */
    const char *value_boolToCString(bool v)
    {
        if (v) {
            static const char *ret = "true";
            return ret;
        } else {
            static const char *ret = "false";
            return ret;
        }
    }

    /*! Converts the given string to double. */
    double value_stringToDouble(const char *s)
    {
        if (strcmp(s, "Infinity") == 0)
            return std::numeric_limits<double>::infinity();
        else if (strcmp(s, "-Infinity") == 0)
            return -std::numeric_limits<double>::infinity();

        return value_stringToDoubleImpl(s);
    }

    /*! Converts the given string to boolean. */
    bool value_stringToBool(const char *s)
    {
        return strlen(s) != 0 && !value_stringsEqual(s, "false") && strcmp(s, "0") != 0;
    }

    /* operations */

    /*! Adds the given values and writes the result to dst. */
    void value_add(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        value_assign_double(dst, value_toDouble(v1) + value_toDouble(v2));
    }

    /*! Subtracts the given values and writes the result to dst. */
    void value_subtract(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        value_assign_double(dst, value_toDouble(v1) - value_toDouble(v2));
    }

    /*! Multiplies the given values and writes the result to dst. */
    void value_multiply(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        value_assign_double(dst, value_toDouble(v1) * value_toDouble(v2));
    }

    /*! Divides the given values and writes the result to dst. */
    void value_divide(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        value_assign_double(dst, value_toDouble(v1) / value_toDouble(v2));
    }

    /*! Calculates the modulo the given values and writes the result to dst. */
    void value_mod(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        double a = value_toDouble(v1);
        double b = value_toDouble(v2);

        if ((b == 0) || std::isinf(a))
            value_assign_special(dst, SpecialValue::NaN);
        else if (std::isinf(b))
            value_assign_double(dst, value_toDouble(v1));
        else if (value_isNegative(v1) || value_isNegative(v2))
            value_assign_double(dst, fmod(value_toDouble(v2) + fmod(value_toDouble(v1), -value_toDouble(v2)), value_toDouble(v2)));
        else
            value_assign_double(dst, fmod(value_toDouble(v1), value_toDouble(v2)));
    }

    /* comparison */

    /*! Returns true if the given values are equal. */
    bool value_equals(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2)
    {
        // https://github.com/scratchfoundation/scratch-vm/blob/112989da0e7306eeb405a5c52616e41c2164af24/src/util/cast.js#L121-L150
        assert(v1 && v2);

        if (v1->type == ValueType::Number && v2->type == ValueType::Number)
            return v1->numberValue == v2->numberValue;
        else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool)
            return v1->boolValue == v2->boolValue;

        bool ok;
        double n1 = value_getNumber(v1, &ok);
        double n2;

        if (ok)
            n2 = value_getNumber(v2, &ok);

        if (!ok) {
            // At least one argument can't be converted to a number
            // Scratch compares strings as case insensitive
            std::u16string s1, s2;
            value_toUtf16(v1, &s1);
            value_toUtf16(v2, &s2);
            return value_u16StringsEqual(s1, s2);
        }

        // Handle the special case of Infinity
        if ((static_cast<int>(v1->type) < 0) && (static_cast<int>(v2->type) < 0)) {
            assert(v1->type != ValueType::NaN);
            assert(v2->type != ValueType::NaN);
            return v1->type == v2->type;
        }

        // Compare as numbers
        return n1 == n2;
    }

    /*! Returns true if the first value is greater than the second value. */
    bool value_greater(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2)
    {
        assert(v1 && v2);

        if (v1->type == ValueType::Number && v2->type == ValueType::Number)
            return v1->numberValue > v2->numberValue;
        else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool)
            return v1->boolValue > v2->boolValue;

        double n1, n2;

        if (v1->type == ValueType::String)
            n1 = value_stringToDouble(v1->stringValue);
        else
            n1 = value_toDouble(v1);

        if (v2->type == ValueType::String)
            n2 = value_stringToDouble(v2->stringValue);
        else
            n2 = value_toDouble(v2);

        return n1 > n2;
    }

    /*! Returns true if the first value is lower than the second value. */
    bool value_lower(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2)
    {
        assert(v1 && v2);

        if (v1->type == ValueType::Number && v2->type == ValueType::Number)
            return v1->numberValue < v2->numberValue;
        else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool)
            return v1->boolValue < v2->boolValue;

        return value_toDouble(v1) < value_toDouble(v2);
        double n1, n2;

        if (v1->type == ValueType::String)
            n1 = value_stringToDouble(v1->stringValue);
        else
            n1 = value_toDouble(v1);

        if (v2->type == ValueType::String)
            n2 = value_stringToDouble(v2->stringValue);
        else
            n2 = value_toDouble(v2);

        return n1 < n2;
    }
}

} // namespace libscratchcpp
