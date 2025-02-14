// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "valuedata.h"

namespace libscratchcpp
{

/*! A typedef for unsigned int. Holds the RGBA values. */
using Rgb = unsigned int;

/*! Returns the red component of the quadruplet rgb. */
inline constexpr int red(Rgb rgb)
{
    return ((rgb >> 16) & 0xff);
}

/*! Returns the green component of the quadruplet rgb. */
inline constexpr int green(Rgb rgb)
{
    return ((rgb >> 8) & 0xff);
}

/*! Returns the blue component of the quadruplet rgb. */
inline constexpr int blue(Rgb rgb)
{
    return (rgb & 0xff);
}

/*! Returns the alpha component of the quadruplet rgb. */
inline constexpr int alpha(Rgb rgb)
{
    return rgb >> 24;
}

/*! Creates an RGB triplet from the given color components. */
inline constexpr Rgb rgb(int r, int g, int b)
{
    return (0xffu << 24) | ((r & 0xffu) << 16) | ((g & 0xffu) << 8) | (b & 0xffu);
}

/*! Creates an RGBA quadruplet from the given color components. */
inline constexpr Rgb rgba(int r, int g, int b, int a)
{
    return ((a & 0xffu) << 24) | ((r & 0xffu) << 16) | ((g & 0xffu) << 8) | (b & 0xffu);
}

extern "C"
{
    LIBSCRATCHCPP_EXPORT void value_free(ValueData *v);

    LIBSCRATCHCPP_EXPORT void value_init(ValueData *v);

    LIBSCRATCHCPP_EXPORT void value_assign_double(ValueData *v, double numberValue);
    LIBSCRATCHCPP_EXPORT void value_assign_bool(ValueData *v, bool boolValue);
    LIBSCRATCHCPP_EXPORT void value_assign_string(ValueData *v, const std::string &stringValue);
    LIBSCRATCHCPP_EXPORT void value_assign_cstring(ValueData *v, const char *stringValue);
    LIBSCRATCHCPP_EXPORT void value_assign_stringPtr(ValueData *v, const StringPtr *stringValue);
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
    LIBSCRATCHCPP_EXPORT StringPtr *value_toStringPtr(const ValueData *v);
    LIBSCRATCHCPP_EXPORT void value_toUtf16(const ValueData *v, std::u16string *dst);
    LIBSCRATCHCPP_EXPORT Rgb value_toRgba(const ValueData *v);

    LIBSCRATCHCPP_EXPORT bool value_doubleIsInt(double v);

    LIBSCRATCHCPP_EXPORT StringPtr *value_doubleToStringPtr(double v);
    LIBSCRATCHCPP_EXPORT const StringPtr *value_boolToStringPtr(bool v);
    LIBSCRATCHCPP_EXPORT double value_stringToDouble(const StringPtr *s);
    LIBSCRATCHCPP_EXPORT bool value_stringToBool(const StringPtr *s);

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
