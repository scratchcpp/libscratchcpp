// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <algorithm>
#include <limits>
#include <ctgmath>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <clocale>

#include "value_functions.h"

// NOTE: Put all logic in value functions so we can keep using this class in unit tests

namespace libscratchcpp
{

/*! \brief The Value class represents a Scratch value. */
class LIBSCRATCHCPP_EXPORT Value
{
    public:
        /*! Constructs a number Value. */
        Value(float numberValue)
        {
            value_init(&m_data);
            value_assign_float(&m_data, numberValue);
        }

        /*! Constructs a number Value. */
        Value(double numberValue)
        {
            value_init(&m_data);
            value_assign_double(&m_data, numberValue);
        }

        /*! Constructs a number Value. */
        Value(int numberValue = 0)
        {
            value_init(&m_data);
            value_assign_int(&m_data, numberValue);
        }

        /*! Constructs a number Value. */
        Value(size_t numberValue)
        {
            value_init(&m_data);
            value_assign_size_t(&m_data, numberValue);
        }

        /*! Constructs a number Value. */
        Value(long numberValue)
        {
            value_init(&m_data);
            value_assign_long(&m_data, numberValue);
        }

        /*! Constructs a boolean Value. */
        Value(bool boolValue)
        {
            value_init(&m_data);
            value_assign_bool(&m_data, boolValue);
        }

        /*! Constructs a string Value. */
        Value(const std::string &stringValue)
        {
            value_init(&m_data);
            value_assign_string(&m_data, stringValue);
        }

        /*! Constructs a string Value. */
        Value(const char *stringValue)
        {
            value_init(&m_data);
            value_assign_cstring(&m_data, stringValue);
        }

        /*! Constructs a special Value. */
        Value(SpecialValue specialValue)
        {
            value_init(&m_data);
            value_assign_special(&m_data, specialValue);
        }

        /*! Constructs value from ValueData. */
        Value(const ValueData &v)
        {
            value_init(&m_data);
            value_assign_copy(&m_data, &v);
        }

        Value(const Value &v)
        {
            value_init(&m_data);
            value_assign_copy(&m_data, &v.m_data);
        }

        ~Value() { value_free(&m_data); }

        /*!
         * Returns a read-only reference to the data.
         * \note Valid until the Value object is destroyed.
         */
        const ValueData &data() const { return m_data; };

        /*!
         * Returns a read/write reference to the data.
         * \note Valid until the Value object is destroyed.
         */
        ValueData &data() { return m_data; }

        /*! Returns the type of the value. */
        ValueType type() const { return m_data.type; }

        /*! Returns true if the value is infinity. */
        bool isInfinity() const { return value_isInfinity(&m_data); }

        /*! Returns true if the value is negative infinity. */
        bool isNegativeInfinity() const { return value_isNegativeInfinity(&m_data); }

        /*! Returns true if the value is NaN (Not a Number). */
        bool isNaN() const { return value_isNaN(&m_data); }

        /*! Returns true if the value is a number. */
        bool isNumber() const { return value_isNumber(&m_data); }

        /*! Returns true if the value is a number or can be converted to a number. */
        bool isValidNumber() const { return value_isValidNumber(&m_data); }

        /*! Returns true if this value represents a round integer. */
        bool isInt() const { return value_isInt(&m_data); }

        /*! Returns true if the value is a boolean. */
        bool isBool() const { return value_isBool(&m_data); }

        /*! Returns true if the value is a string. */
        bool isString() const { return value_isString(&m_data); }

        /*! Returns the int representation of the value. */
        int toInt() const { return value_toInt(&m_data); }

        /*! Returns the long representation of the value. */
        long toLong() const { return value_toLong(&m_data); }

        /*! Returns the double representation of the value. */
        double toDouble() const { return value_toDouble(&m_data); };

        /*! Returns the boolean representation of the value. */
        bool toBool() const { return value_toBool(&m_data); };

        /*! Returns the string representation of the value. */
        std::string toString() const
        {
            std::string ret;
            value_toString(&m_data, &ret);
            return ret;
        };

        /*! Returns the UTF-16 representation of the value. */
        std::u16string toUtf16()
        {
            std::u16string ret;
            value_toUtf16(&m_data, &ret);
            return ret;
        }

        /*! Adds the given value to the value. */
        void add(const Value &v) { value_add(&m_data, &v.m_data, &m_data); }

        /*! Subtracts the given value from the value. */
        void subtract(const Value &v) { value_subtract(&m_data, &v.m_data, &m_data); }

        /*! Multiplies the given value with the value. */
        void multiply(const Value &v) { value_multiply(&m_data, &v.m_data, &m_data); }

        /*! Divides the value by the given value. */
        void divide(const Value &v) { value_divide(&m_data, &v.m_data, &m_data); }

        /*! Replaces the value with modulo of the value and the given value. */
        void mod(const Value &v) { value_mod(&m_data, &v.m_data, &m_data); }

        const Value &operator=(float v)
        {
            value_assign_float(&m_data, v);
            return *this;
        }

        const Value &operator=(double v)
        {
            value_assign_double(&m_data, v);
            return *this;
        }

        const Value &operator=(int v)
        {
            value_assign_int(&m_data, v);
            return *this;
        }

        const Value &operator=(long v)
        {
            value_assign_long(&m_data, v);
            return *this;
        }

        const Value &operator=(bool v)
        {
            value_assign_bool(&m_data, v);
            return *this;
        }

        const Value &operator=(const std::string &v)
        {
            value_assign_string(&m_data, v);
            return *this;
        }

        const Value &operator=(const char *v)
        {
            value_assign_cstring(&m_data, v);
            return *this;
        }

        const Value &operator=(const ValueData &v)
        {
            value_assign_copy(&m_data, &v);
            return *this;
        }

        const Value &operator=(const Value &v)
        {
            value_assign_copy(&m_data, &v.m_data);
            return *this;
        }

    private:
        ValueData m_data;

        friend bool operator==(const Value &v1, const Value &v2) { return value_equals(&v1.m_data, &v2.m_data); }

        friend bool operator!=(const Value &v1, const Value &v2) { return !value_equals(&v1.m_data, &v2.m_data); }

        friend bool operator>(const Value &v1, const Value &v2) { return value_greater(&v1.m_data, &v2.m_data); }

        friend bool operator<(const Value &v1, const Value &v2) { return value_lower(&v1.m_data, &v2.m_data); }

        friend bool operator>=(const Value &v1, const Value &v2) { return v1 > v2 || v1 == v2; }

        friend bool operator<=(const Value &v1, const Value &v2) { return v1 < v2 || v1 == v2; }

        friend Value operator+(const Value &v1, const Value &v2)
        {
            Value ret;
            value_add(&v1.m_data, &v2.m_data, &ret.m_data);
            return ret;
        }

        friend Value operator-(const Value &v1, const Value &v2)
        {
            Value ret;
            value_subtract(&v1.m_data, &v2.m_data, &ret.m_data);
            return ret;
        }

        friend Value operator*(const Value &v1, const Value &v2)
        {
            Value ret;
            value_multiply(&v1.m_data, &v2.m_data, &ret.m_data);
            return ret;
        }

        friend Value operator/(const Value &v1, const Value &v2)
        {
            Value ret;
            value_divide(&v1.m_data, &v2.m_data, &ret.m_data);
            return ret;
        }

        friend Value operator%(const Value &v1, const Value &v2)
        {
            Value ret;
            value_mod(&v1.m_data, &v2.m_data, &ret.m_data);
            return ret;
        }
};

} // namespace libscratchcpp
