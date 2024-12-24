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
                return true;

            case ValueType::Number:
                return value_doubleIsInt(v->numberValue);

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
        if (v->type == ValueType::Number) {
            return v->numberValue;
        } else if (v->type == ValueType::Bool)
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
        if (v->type == ValueType::Number) {
            return std::isnan(v->numberValue) ? 0 : v->numberValue;
        } else if (v->type == ValueType::Bool)
            return v->boolValue;
        else if (v->type == ValueType::String)
            return value_stringToDouble(v->stringValue);
        else
            return 0;
    }

    /*! Returns the boolean representation of the given value. */
    bool value_toBool(const libscratchcpp::ValueData *v)
    {
        if (v->type == ValueType::Bool) {
            return v->boolValue;
        } else if (v->type == ValueType::Number) {
            return v->numberValue != 0 && !std::isnan(v->numberValue);
        } else if (v->type == ValueType::String) {
            return value_stringToBool(v->stringValue);
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

    /*! Returns true if the given number represents a round integer. */
    bool value_doubleIsInt(double v)
    {
        if (std::isinf(v) || std::isnan(v))
            return true;

        double intpart;
        std::modf(v, &intpart);
        return v == intpart;
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
        const double a = value_toDouble(v1);
        const double b = value_toDouble(v2);
        value_assign_double(dst, fmod(a, b) / b < 0.0 ? fmod(a, b) + b : fmod(a, b));
    }

    /* comparison */

    /*! Returns true if the given values are equal. */
    bool value_equals(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2)
    {
        return value_compare(v1, v2) == 0;
    }

    /*! Returns true if the first value is greater than the second value. */
    bool value_greater(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2)
    {
        return value_compare(v1, v2) > 0;
    }

    /*! Returns true if the first value is lower than the second value. */
    bool value_lower(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2)
    {
        return value_compare(v1, v2) < 0;
    }
}

} // namespace libscratchcpp
