#pragma once

#include "valuedata.h"

namespace libscratchcpp
{

extern "C"
{
    LIBSCRATCHCPP_EXPORT void value_free(ValueData *v);

    LIBSCRATCHCPP_EXPORT void value_init(ValueData *v);

    LIBSCRATCHCPP_EXPORT void value_assign_double(ValueData *v, double numberValue);
    LIBSCRATCHCPP_EXPORT void value_assign_bool(ValueData *v, bool boolValue);
    LIBSCRATCHCPP_EXPORT void value_assign_string(ValueData *v, const std::string &stringValue);
    LIBSCRATCHCPP_EXPORT void value_assign_cstring(ValueData *v, const char *stringValue);
    LIBSCRATCHCPP_EXPORT void value_assign_copy(ValueData *v, const ValueData *another);

    LIBSCRATCHCPP_EXPORT bool value_isInfinity(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_isNegativeInfinity(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_isNaN(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_isNumber(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_isValidNumber(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_isInt(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_isBool(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_isString(const ValueData *v);

    LIBSCRATCHCPP_EXPORT long value_toLong(const ValueData *v);
    LIBSCRATCHCPP_EXPORT int value_toInt(const ValueData *v);
    LIBSCRATCHCPP_EXPORT double value_toDouble(const ValueData *v);
    LIBSCRATCHCPP_EXPORT bool value_toBool(const ValueData *v);
    LIBSCRATCHCPP_EXPORT void value_toString(const ValueData *v, std::string *dst);
    LIBSCRATCHCPP_EXPORT char *value_toCString(const ValueData *v);
    LIBSCRATCHCPP_EXPORT void value_toUtf16(const ValueData *v, std::u16string *dst);

    LIBSCRATCHCPP_EXPORT bool value_doubleIsInt(double v);

    LIBSCRATCHCPP_EXPORT char *value_doubleToCString(double v);
    LIBSCRATCHCPP_EXPORT const char *value_boolToCString(bool v);
    LIBSCRATCHCPP_EXPORT double value_stringToDouble(const char *s);
    LIBSCRATCHCPP_EXPORT bool value_stringToBool(const char *s);

    LIBSCRATCHCPP_EXPORT void value_add(const ValueData *v1, const ValueData *v2, ValueData *dst);
    LIBSCRATCHCPP_EXPORT void value_subtract(const ValueData *v1, const ValueData *v2, ValueData *dst);
    LIBSCRATCHCPP_EXPORT void value_multiply(const ValueData *v1, const ValueData *v2, ValueData *dst);
    LIBSCRATCHCPP_EXPORT void value_divide(const ValueData *v1, const ValueData *v2, ValueData *dst);
    LIBSCRATCHCPP_EXPORT void value_mod(const ValueData *v1, const ValueData *v2, ValueData *dst);

    LIBSCRATCHCPP_EXPORT bool value_equals(const ValueData *v1, const ValueData *v2);
    LIBSCRATCHCPP_EXPORT bool value_greater(const ValueData *v1, const ValueData *v2);
    LIBSCRATCHCPP_EXPORT bool value_lower(const ValueData *v1, const ValueData *v2);
}

} // namespace libscratchcpp
