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
    void value_free(ValueData *v);

    void value_init(ValueData *v);

    void value_assign_double(ValueData *v, double numberValue);
    void value_assign_bool(ValueData *v, bool boolValue);
    void value_assign_string(ValueData *v, const std::string &stringValue);
    void value_assign_cstring(ValueData *v, const char *stringValue);
    void value_assign_stringPtr(ValueData *v, const StringPtr *stringValue);
    void value_assign_pointer(ValueData *v, const void *pointerValue);
    void value_assign_copy(ValueData *v, const ValueData *another);

    bool value_isInfinity(const ValueData *v);
    bool value_isNegativeInfinity(const ValueData *v);
    bool value_isNaN(const ValueData *v);
    bool value_isNumber(const ValueData *v);
    bool value_isValidNumber(const ValueData *v);
    bool value_isInt(const ValueData *v);
    bool value_isBool(const ValueData *v);
    bool value_isString(const ValueData *v);
    bool value_isPointer(const ValueData *v);

    long value_toLong(const ValueData *v);
    int value_toInt(const ValueData *v);
    double value_toDouble(const ValueData *v);
    bool value_toBool(const ValueData *v);
    void value_toString(const ValueData *v, std::string *dst);
    void value_toStringPtr(const ValueData *v, StringPtr *dst);
    void value_toUtf16(const ValueData *v, std::u16string *dst);
    Rgb value_toRgba(const ValueData *v);
    const void *value_toPointer(const ValueData *v);

    bool value_doubleIsInt(double v);

    void value_doubleToStringPtr(double v, StringPtr *dst);
    const StringPtr *value_boolToStringPtr(bool v);
    double value_stringToDouble(const StringPtr *s);
    double value_stringToDoubleWithCheck(const StringPtr *s, bool *ok);
    bool value_stringToBool(const StringPtr *s);

    void value_add(const ValueData *v1, const ValueData *v2, ValueData *dst);
    void value_subtract(const ValueData *v1, const ValueData *v2, ValueData *dst);
    void value_multiply(const ValueData *v1, const ValueData *v2, ValueData *dst);
    void value_divide(const ValueData *v1, const ValueData *v2, ValueData *dst);
    void value_mod(const ValueData *v1, const ValueData *v2, ValueData *dst);

    bool value_equals(const ValueData *v1, const ValueData *v2);
    bool value_greater(const ValueData *v1, const ValueData *v2);
    bool value_lower(const ValueData *v1, const ValueData *v2);
}

} // namespace libscratchcpp
