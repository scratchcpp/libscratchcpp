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
            delete v->stringValue;
            v->stringValue = nullptr;
        }
    }

    /* init */

    /*! Initializes the given value. */
    void value_init(ValueData *v)
    {
        v->type = ValueType::Integer;
        v->intValue = 0;
    }

    /* assign */

    /*! Assigns number of type 'float' to the given value. */
    void value_assign_float(ValueData *v, float numberValue)
    {
        value_free(v);

        if (value_isInf(numberValue))
            v->type = ValueType::Infinity;
        else if (value_isNegativeInf(numberValue))
            v->type = ValueType::NegativeInfinity;
        else if (std::isnan(numberValue))
            v->type = ValueType::NaN;
        else {
            v->type = ValueType::Double;
            v->doubleValue = value_floatToDouble(numberValue);
        }
    }

    /*! Assigns number of type 'double' to the given value. */
    void value_assign_double(ValueData *v, double numberValue)
    {
        value_free(v);

        if (value_isInf(numberValue))
            v->type = ValueType::Infinity;
        else if (value_isNegativeInf(numberValue))
            v->type = ValueType::NegativeInfinity;
        else if (std::isnan(numberValue))
            v->type = ValueType::NaN;
        else {
            v->type = ValueType::Double;
            v->doubleValue = numberValue;
        }
    }

    /*! Assigns number of type 'int' to the given value. */
    void value_assign_int(ValueData *v, int numberValue)
    {
        value_free(v);

        v->type = ValueType::Integer;
        v->intValue = numberValue;
    }

    /*! Assigns number of type 'size_t' to the given value. */
    void value_assign_size_t(ValueData *v, size_t numberValue)
    {
        value_free(v);

        v->type = ValueType::Integer;
        v->intValue = numberValue;
    }

    /*! Assigns number of type 'long' to the given value. */
    void value_assign_long(ValueData *v, long numberValue)
    {
        value_free(v);

        v->type = ValueType::Integer;
        v->intValue = numberValue;
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
        if (stringValue == "Infinity") {
            value_free(v);
            v->type = ValueType::Infinity;
        } else if (stringValue == "-Infinity") {
            value_free(v);
            v->type = ValueType::NegativeInfinity;
        } else if (stringValue == "NaN") {
            value_free(v);
            v->type = ValueType::NaN;
        } else {
            if (v->type == ValueType::String)
                v->stringValue->assign(stringValue);
            else {
                value_free(v);
                v->type = ValueType::String;
                v->stringValue = new std::string(stringValue);
            }
        }
    }

    /*! Assigns C string to the given value. */
    void value_assign_cstring(ValueData *v, const char *stringValue)
    {
        value_assign_string(v, std::string(stringValue));
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
            v->type = ValueType::Integer;
            v->intValue = 0;
        }
    }

    /*! Assigns another value to the given value. */
    void value_assign_copy(ValueData *v, const libscratchcpp::ValueData *another)
    {
        if (another->type == ValueType::Integer) {
            value_free(v);
            v->intValue = another->intValue;
        } else if (another->type == ValueType::Double) {
            value_free(v);
            v->doubleValue = another->doubleValue;
        } else if (another->type == ValueType::Bool) {
            value_free(v);
            v->boolValue = another->boolValue;
        } else if (another->type == ValueType::String) {
            if (v->type == ValueType::String)
                v->stringValue->assign(*another->stringValue);
            else {
                value_free(v);
                v->stringValue = new std::string(*another->stringValue);
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
            case ValueType::Integer:
                return value_isInf(v->intValue);
            case ValueType::Double:
                return value_isInf(v->doubleValue);
            case ValueType::String:
                return *v->stringValue == "Infinity";
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
            case ValueType::Integer:
                return value_isNegativeInf(-v->intValue);
            case ValueType::Double:
                return value_isNegativeInf(-v->doubleValue);
            case ValueType::String:
                return *v->stringValue == "-Infinity";
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
            case ValueType::Double:
                assert(!std::isnan(v->doubleValue));
                return std::isnan(v->doubleValue);
            case ValueType::String:
                return *v->stringValue == "NaN";
            default:
                return false;
        }
    }

    /*! Returns true if the given value is a number. */
    bool value_isNumber(const libscratchcpp::ValueData *v)
    {
        return v->type == ValueType::Integer || v->type == ValueType::Double;
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
            case ValueType::Integer:
            case ValueType::Double:
            case ValueType::Bool:
                return true;
            case ValueType::String:
                return v->stringValue->empty() || value_checkString(*v->stringValue) > 0;
            default:
                return false;
        }
    }

    /*! Returns true if the given value represents a round integer. */
    bool value_isInt(const ValueData *v)
    {
        // https://github.com/scratchfoundation/scratch-vm/blob/112989da0e7306eeb405a5c52616e41c2164af24/src/util/cast.js#L157-L181
        switch (v->type) {
            case ValueType::Integer:
            case ValueType::Bool:
            case ValueType::Infinity:
            case ValueType::NegativeInfinity:
            case ValueType::NaN:
                return true;
            case ValueType::Double: {
                double intpart;
                std::modf(v->doubleValue, &intpart);
                return v->doubleValue == intpart;
            }
            case ValueType::String:
                return v->stringValue->find('.') == std::string::npos;
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
        if (v->type == ValueType::Integer)
            return v->intValue;
        else if (v->type == ValueType::Double)
            return v->doubleValue;
        else if (v->type == ValueType::Bool)
            return v->boolValue;
        else if (v->type == ValueType::String)
            return value_stringToLong(*v->stringValue);
        else
            return 0;
    }

    /*! Returns the int representation of the given value. */
    int value_toInt(const libscratchcpp::ValueData *v)
    {
        if (v->type == ValueType::Integer)
            return v->intValue;
        else if (v->type == ValueType::Double)
            return v->doubleValue;
        else if (v->type == ValueType::Bool)
            return v->boolValue;
        else if (v->type == ValueType::String)
            return value_stringToLong(*v->stringValue);
        else
            return 0;
    }

    /*! Returns the double representation of the given value. */
    double value_toDouble(const libscratchcpp::ValueData *v)
    {
        if (v->type == ValueType::Double)
            return v->doubleValue;
        else if (v->type == ValueType::Integer)
            return v->intValue;
        else if (v->type == ValueType::Bool)
            return v->boolValue;
        else if (v->type == ValueType::String)
            return value_stringToDouble(*v->stringValue);
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
        } else if (v->type == ValueType::Integer) {
            return v->intValue != 0;
        } else if (v->type == ValueType::Double) {
            return v->doubleValue != 0;
        } else if (v->type == ValueType::String) {
            return !v->stringValue->empty() && !value_stringsEqual(*v->stringValue, "false") && *v->stringValue != "0";
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
            dst->assign(*v->stringValue);
        else if (v->type == ValueType::Integer)
            dst->assign(std::to_string(v->intValue));
        else if (v->type == ValueType::Double)
            dst->assign(value_doubleToString(v->doubleValue));
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

    /*! Writes the UTF-16 representation of the given value to dst. */
    void value_toUtf16(const libscratchcpp::ValueData *v, std::u16string *dst)
    {
        std::string s;
        value_toString(v, &s);
        dst->assign(utf8::utf8to16(s));
    }

    /* operations */

    /*! Adds the given values and writes the result to dst. */
    void value_add(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        if (v1->type == ValueType::Integer && v2->type == ValueType::Integer) {
            value_assign_long(dst, v1->intValue + v2->intValue);
            return;
        } else if (v1->type == ValueType::Double && v2->type == ValueType::Double) {
            value_assign_double(dst, v1->doubleValue + v2->doubleValue);
            return;
        } else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool) {
            value_assign_long(dst, v1->boolValue + v2->boolValue);
            return;
        } else if ((static_cast<int>(v1->type) < 0) || (static_cast<int>(v2->type) < 0)) {
            if ((v1->type == ValueType::Infinity && v2->type == ValueType::NegativeInfinity) || (v1->type == ValueType::NegativeInfinity && v2->type == ValueType::Infinity)) {
                value_assign_special(dst, SpecialValue::NaN);
                return;
            } else if (v1->type == ValueType::Infinity || v2->type == ValueType::Infinity) {
                value_assign_special(dst, SpecialValue::Infinity);
                return;
            } else if (v1->type == ValueType::NegativeInfinity || v2->type == ValueType::NegativeInfinity) {
                value_assign_special(dst, SpecialValue::NegativeInfinity);
                return;
            }
        }

        value_assign_double(dst, value_toDouble(v1) + value_toDouble(v2));
    }

    /*! Subtracts the given values and writes the result to dst. */
    void value_subtract(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        if (v1->type == ValueType::Integer && v2->type == ValueType::Integer) {
            value_assign_long(dst, v1->intValue - v2->intValue);
            return;
        } else if (v1->type == ValueType::Double && v2->type == ValueType::Double) {
            value_assign_double(dst, v1->doubleValue - v2->doubleValue);
            return;
        } else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool) {
            value_assign_long(dst, v1->boolValue - v2->boolValue);
            return;
        } else if ((static_cast<int>(v1->type) < 0) || (static_cast<int>(v2->type) < 0)) {
            if ((v1->type == ValueType::Infinity && v2->type == ValueType::Infinity) || (v1->type == ValueType::NegativeInfinity && v2->type == ValueType::NegativeInfinity)) {
                value_assign_special(dst, SpecialValue::NaN);
                return;
            } else if (v1->type == ValueType::Infinity || v2->type == ValueType::NegativeInfinity) {
                value_assign_special(dst, SpecialValue::Infinity);
                return;
            } else if (v1->type == ValueType::NegativeInfinity || v2->type == ValueType::Infinity) {
                value_assign_special(dst, SpecialValue::NegativeInfinity);
                return;
            }
        }

        value_assign_double(dst, value_toDouble(v1) - value_toDouble(v2));
    }

    /*! Multiplies the given values and writes the result to dst. */
    void value_multiply(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        if (v1->type == ValueType::Integer && v2->type == ValueType::Integer)
            value_assign_long(dst, v1->intValue * v2->intValue);
        else if (v1->type == ValueType::Double && v2->type == ValueType::Double)
            value_assign_double(dst, v1->doubleValue * v2->doubleValue);
        else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool)
            value_assign_long(dst, v1->boolValue * v2->boolValue);
        else {
            const ValueType t1 = v1->type, t2 = v2->type;

            if ((static_cast<int>(t1) < 0 && t1 != ValueType::NaN) || (static_cast<int>(t2) < 0 && t2 != ValueType::NaN)) {
                if (t1 == ValueType::Infinity || t1 == ValueType::NegativeInfinity || t2 == ValueType::Infinity || t2 == ValueType::NegativeInfinity) {
                    bool mode = (t1 == ValueType::Infinity || t2 == ValueType::Infinity);
                    const ValueData *value;

                    if ((t1 == ValueType::Infinity && (t2 == ValueType::Infinity || t2 == ValueType::NegativeInfinity)) || (t2 != ValueType::Infinity && t2 != ValueType::NegativeInfinity))
                        value = v2;
                    else
                        value = v1;

                    if (value_isPositive(value))
                        value_assign_special(dst, mode ? SpecialValue::Infinity : SpecialValue::NegativeInfinity);
                    else if (value_isNegative(value))
                        value_assign_special(dst, mode ? SpecialValue::NegativeInfinity : SpecialValue::Infinity);
                    else
                        value_assign_special(dst, SpecialValue::NaN);
                }
            } else
                value_assign_double(dst, value_toDouble(v1) * value_toDouble(v2));
        }
    }

    /*! Divides the given values and writes the result to dst. */
    void value_divide(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        if (value_isZero(v1) && value_isZero(v2))
            value_assign_special(dst, SpecialValue::NaN);
        else if (value_toDouble(v2) == 0) {
            if (value_isPositive(v1))
                value_assign_special(dst, SpecialValue::Infinity);
            else
                value_assign_special(dst, SpecialValue::NegativeInfinity);
        } else if ((v1->type == ValueType::Infinity || v1->type == ValueType::NegativeInfinity) && (v2->type == ValueType::Infinity || v2->type == ValueType::NegativeInfinity)) {
            value_assign_special(dst, SpecialValue::NaN);
        } else if (v1->type == ValueType::Infinity || v1->type == ValueType::NegativeInfinity) {
            if (value_toDouble(v2) < 0) {
                if (v1->type == ValueType::Infinity)
                    value_assign_special(dst, SpecialValue::NegativeInfinity);
                else
                    value_assign_special(dst, SpecialValue::Infinity);
            } else {
                if (v1->type == ValueType::Infinity)
                    value_assign_special(dst, SpecialValue::Infinity);
                else
                    value_assign_special(dst, SpecialValue::NegativeInfinity);
            }
        } else if (v2->type == ValueType::Infinity || v2->type == ValueType::NegativeInfinity) {
            value_assign_long(dst, 0);
        } else
            value_assign_double(dst, value_toDouble(v1) / value_toDouble(v2));
    }

    /*! Calculates the modulo the given values and writes the result to dst. */
    void value_mod(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2, ValueData *dst)
    {
        if ((v2 == 0) || (v1->type == ValueType::Infinity || v1->type == ValueType::NegativeInfinity))
            value_assign_special(dst, SpecialValue::NaN);
        else if (v2->type == ValueType::Infinity || v2->type == ValueType::NegativeInfinity)
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

        if (v1->type == ValueType::Integer && v2->type == ValueType::Integer)
            return v1->intValue == v2->intValue;
        else if (v1->type == ValueType::Double && v2->type == ValueType::Double)
            return v1->doubleValue == v2->doubleValue;
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

        if (v1->type == ValueType::Integer && v2->type == ValueType::Integer)
            return v1->intValue > v2->intValue;
        else if (v1->type == ValueType::Double && v2->type == ValueType::Double)
            return v1->doubleValue > v2->doubleValue;
        else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool)
            return v1->boolValue > v2->boolValue;
        else if ((static_cast<int>(v1->type) < 0) || (static_cast<int>(v2->type) < 0)) {
            if (v1->type == ValueType::Infinity) {
                return v2->type != ValueType::Infinity;
            } else if (v1->type == ValueType::NegativeInfinity)
                return false;
            else if (v2->type == ValueType::Infinity)
                return false;
            else if (v2->type == ValueType::NegativeInfinity)
                return true;
        }

        return value_toDouble(v1) > value_toDouble(v2);
    }

    /*! Returns true if the first value is lower than the second value. */
    bool value_lower(const libscratchcpp::ValueData *v1, const libscratchcpp::ValueData *v2)
    {
        assert(v1 && v2);

        if (v1->type == ValueType::Integer && v2->type == ValueType::Integer)
            return v1->intValue < v2->intValue;
        else if (v1->type == ValueType::Double && v2->type == ValueType::Double)
            return v1->doubleValue < v2->doubleValue;
        else if (v1->type == ValueType::Bool && v2->type == ValueType::Bool)
            return v1->boolValue < v2->boolValue;
        else if ((static_cast<int>(v1->type) < 0) || (static_cast<int>(v2->type) < 0)) {
            if (v1->type == ValueType::Infinity) {
                return false;
            } else if (v1->type == ValueType::NegativeInfinity)
                return v2->type != ValueType::NegativeInfinity;
            else if (v2->type == ValueType::Infinity)
                return v1->type != ValueType::Infinity;
            else if (v2->type == ValueType::NegativeInfinity)
                return false;
        }

        return value_toDouble(v1) < value_toDouble(v2);
    }
}

} // namespace libscratchcpp
