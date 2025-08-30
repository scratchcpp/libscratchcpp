#include <scratchcpp/string_pool.h>
#include <scratchcpp/stringptr.h>
#include <ctgmath>
#include <cassert>
#include <clocale>
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
            string_pool_free(v->stringValue);
            v->stringValue = nullptr;
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
        if (v->type != ValueType::String) {
            v->stringValue = string_pool_new();
            v->type = ValueType::String;
        }

        string_assign_cstring(v->stringValue, stringValue);
    }

    /*! Assigns string to the given value. */
    void value_assign_stringPtr(ValueData *v, const StringPtr *stringValue)
    {
        if (v->type != ValueType::String) {
            v->stringValue = string_pool_new();
            v->type = ValueType::String;
        }

        string_assign(v->stringValue, stringValue);
    }

    /*! Assigns pointer to the given value. */
    void value_assign_pointer(ValueData *v, const void *pointerValue)
    {
        value_free(v);

        v->type = ValueType::Pointer;
        v->pointerValue = pointerValue;
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
                string_assign(v->stringValue, another->stringValue);
            else {
                v->stringValue = string_pool_new();
                string_assign(v->stringValue, another->stringValue);
                v->type = ValueType::String;
            }
        } else if (another->type == ValueType::Pointer) {
            value_free(v);
            v->pointerValue = another->pointerValue;
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
                return string_compare_case_sensitive(v->stringValue, &INFINITY_STR) == 0;
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
                return string_compare_case_sensitive(v->stringValue, &NEGATIVE_INFINITY_STR) == 0;
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
                return string_compare_case_sensitive(v->stringValue, &NAN_STR) == 0;
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
                return v->stringValue->size == 0 || value_checkString(v->stringValue) > 0;
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

            case ValueType::Pointer:
                return false;
        }

        return false;
    }

    /*! Returns true if the given value is a pointer. */
    bool value_isPointer(const ValueData *v)
    {
        return v->type == ValueType::Pointer;
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
            return std::isnan(v->numberValue) || std::isinf(v->numberValue) ? 0 : v->numberValue;
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
            return std::isnan(v->numberValue) || std::isinf(v->numberValue) ? 0 : v->numberValue;
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
        } else if (v->type == ValueType::Pointer) {
            return v->pointerValue;
        } else {
            return false;
        }
    }

    /*! Writes the string representation of the given value to dst. */
    void value_toString(const ValueData *v, std::string *dst)
    {
        StringPtr *str = string_pool_new();
        value_toStringPtr(v, str);
        dst->assign(utf8::utf16to8(std::u16string(str->data)));
        string_pool_free(str);
    }

    /*! Writes the string representation of the given value to dst. */
    void value_toStringPtr(const ValueData *v, StringPtr *dst)
    {
        if (v->type == ValueType::String)
            string_assign(dst, v->stringValue);
        else if (v->type == ValueType::Number)
            value_doubleToStringPtr(v->numberValue, dst);
        else if (v->type == ValueType::Bool) {
            const StringPtr *boolStr = value_boolToStringPtr(v->boolValue);
            string_assign(dst, boolStr);
        } else
            string_assign(dst, &ZERO_STR);
    }

    /*! Writes the UTF-16 representation of the given value to dst. */
    void value_toUtf16(const libscratchcpp::ValueData *v, std::u16string *dst)
    {
        StringPtr *str = string_pool_new();
        value_toStringPtr(v, str);
        dst->assign(str->data);
        string_pool_free(str);
    }

    /*! Returns the RGBA quadruplet from the given color value. */
    Rgb value_toRgba(const ValueData *v)
    {
        // https://github.com/scratchfoundation/scratch-vm/blob/112989da0e7306eeb405a5c52616e41c2164af24/src/util/cast.js#L92-L103
        StringPtr *string = nullptr;

        if (v->type == ValueType::Number)
            return v->numberValue;
        else if (v->type == ValueType::String)
            string = v->stringValue;
        else if (v->type == ValueType::Bool)
            return v->boolValue;
        else
            return 0;

        if (string->size > 0 && string->data[0] == u'#') {
            // https://github.com/scratchfoundation/scratch-vm/blob/a4f095db5e03e072ba222fe721eeeb543c9b9c15/src/util/color.js#L60-L69
            // (this implementation avoids regex)

            // Handle shorthand hex (e.g., "abc" -> "aabbcc")
            char16_t expandedHex[7] = { 0 };
            char16_t *ptr;

            if (string->size == 4) {
                expandedHex[0] = string->data[1];
                expandedHex[1] = string->data[1];
                expandedHex[2] = string->data[2];
                expandedHex[3] = string->data[2];
                expandedHex[4] = string->data[3];
                expandedHex[5] = string->data[3];
                ptr = expandedHex;
            } else if (string->size == 7)
                ptr = string->data + 1; // skip '#'
            else
                return rgb(0, 0, 0);

            // Convert hex components to integers
            int r, g, b;

            // TODO: Do not use sscanf()
            std::string str = utf8::utf16to8(std::u16string(ptr));

            if (std::sscanf(str.c_str(), "%2x%2x%2x", &r, &g, &b) == 3)
                return rgb(r, g, b);
        } else if (string->size > 0) {
            const double ret = value_stringToDouble(string);
            return ret;
        }

        return rgb(0, 0, 0);
    }

    /*! Returns the pointer stored in the given value or nullptr if it isn't a pointer. */
    const void *value_toPointer(const ValueData *v)
    {
        if (v->type == ValueType::Pointer)
            return v->pointerValue;
        else
            return nullptr;
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

    /*! Converts the given number to string and stores the result in dst. */
    void value_doubleToStringPtr(double v, StringPtr *dst)
    {
        if (v == 0)
            string_assign_cstring(dst, "0");
        else if (std::isinf(v)) {
            if (v > 0)
                string_assign(dst, &INFINITY_STR);
            else
                string_assign(dst, &NEGATIVE_INFINITY_STR);
        } else if (std::isnan(v))
            string_assign(dst, &NAN_STR);
        else {
            // snprintf() is locale-dependent
            std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
            std::setlocale(LC_NUMERIC, "C");

            const int maxlen = 26; // should be enough for any number
            char *buffer = (char *)malloc((maxlen + 1) * sizeof(char));

            // Constants for significant digits and thresholds
            const int significantDigits = 17 - value_intDigitCount(std::floor(std::fabs(v)));
            constexpr double scientificThreshold = 1e21;
            constexpr double minScientificThreshold = 1e-6;

            // Use scientific notation if the number is very large or very small
            if (std::fabs(v) >= scientificThreshold || std::fabs(v) < minScientificThreshold) {
                int ret = snprintf(buffer, maxlen, "%.*g", significantDigits - 1, v);
                assert(ret >= 0);
            } else {
                snprintf(buffer, maxlen, "%.*f", significantDigits - 1, v);

                // Remove trailing zeros from the fractional part
                char *dot = std::strchr(buffer, '.');

                if (dot) {
                    char *end = buffer + std::strlen(buffer) - 1;
                    while (end > dot && *end == '0') {
                        *end-- = '\0';
                    }
                    if (*end == '.') {
                        *end = '\0'; // Remove trailing dot
                    }
                }
            }

            // Remove leading zeros after e+/e-
            for (int i = 0; i < 2; i++) {
                const char *target = (i == 0) ? "e+" : "e-";
                char *index = strstr(buffer, target);

                if (index != nullptr) {
                    char *ptr = index + 2;
                    while (*(ptr + 1) != '\0' && *ptr == '0') {
                        // Shift the characters left to erase '0'
                        char *shiftPtr = ptr;
                        do {
                            *shiftPtr = *(shiftPtr + 1);
                            shiftPtr++;
                        } while (*shiftPtr != '\0');
                    }
                }
            }

            // Restore old locale
            std::setlocale(LC_NUMERIC, oldLocale.c_str());

            string_assign_cstring(dst, buffer);
            free(buffer);
        }
    }

    /*!
     * Converts the given boolean to string.
     * \note Do not free the string!
     */
    const StringPtr *value_boolToStringPtr(bool v)
    {
        return v ? &TRUE_STR : &FALSE_STR;
    }

    /*! Converts the given string to double. */
    double value_stringToDouble(const StringPtr *s)
    {
        if (string_compare_case_sensitive(s, &INFINITY_STR) == 0)
            return std::numeric_limits<double>::infinity();
        else if (string_compare_case_sensitive(s, &NEGATIVE_INFINITY_STR) == 0)
            return -std::numeric_limits<double>::infinity();

        return value_stringToDoubleImpl(s->data, s->size);
    }

    /*!
     * Converts the given string to double.
     * \param[out] ok Whether the conversion was successful.
     */
    double value_stringToDoubleWithCheck(const StringPtr *s, bool *ok)
    {
        *ok = true;

        if (string_compare_case_sensitive(s, &INFINITY_STR) == 0)
            return std::numeric_limits<double>::infinity();
        else if (string_compare_case_sensitive(s, &NEGATIVE_INFINITY_STR) == 0)
            return -std::numeric_limits<double>::infinity();

        return value_stringToDoubleImpl(s->data, s->size, ok);
    }

    /*! Converts the given string to boolean. */
    bool value_stringToBool(const StringPtr *s)
    {
        return s->size != 0 && string_compare_case_insensitive(s, &FALSE_STR) != 0 && string_compare_case_insensitive(s, &ZERO_STR) != 0;
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
