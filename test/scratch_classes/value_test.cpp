#include <scratchcpp/value.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/string_functions.h>
#include <utf8.h>

#include "../common.h"

using namespace libscratchcpp;

static StringPtr *string_pool_new_assign(const char *str)
{
    StringPtr *ret = string_pool_new();
    string_assign_cstring(ret, str);
    return ret;
}

TEST(ValueTest, DefaultConstructor)
{
    Value v;
    ASSERT_EQ(v.toDouble(), 0);
    ASSERT_EQ(v.type(), ValueType::Number);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_TRUE(v.isValidNumber());
    ASSERT_TRUE(v.isInt());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, DoubleConstructor)
{
    {
        Value v(static_cast<double>(3.14));
        ASSERT_EQ(v.toDouble(), 3.14);
        ASSERT_EQ(v.type(), ValueType::Number);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_TRUE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(static_cast<double>(-5.0));
        ASSERT_EQ(v.toDouble(), -5.0);
        ASSERT_EQ(v.type(), ValueType::Number);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_TRUE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_TRUE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(std::numeric_limits<double>::infinity());
        ASSERT_EQ(v.type(), ValueType::Number);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_TRUE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(-std::numeric_limits<double>::infinity());
        ASSERT_EQ(v.type(), ValueType::Number);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_TRUE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(std::numeric_limits<double>::quiet_NaN());
        ASSERT_EQ(v.type(), ValueType::Number);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_TRUE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, BoolConstructor)
{
    {
        Value v(true);
        ASSERT_EQ(v.toBool(), true);
        ASSERT_EQ(v.type(), ValueType::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_TRUE(v.isInt());
        ASSERT_TRUE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(false);
        ASSERT_EQ(v.toBool(), false);
        ASSERT_EQ(v.type(), ValueType::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_TRUE(v.isInt());
        ASSERT_TRUE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, StdStringConstructor)
{
    {
        Value v(std::string("test"));
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("532"));
        ASSERT_EQ(v.toString(), "532");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_TRUE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("532.15"));

        ASSERT_EQ(v.toString(), "532.15");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("1 2 3"));
        ASSERT_EQ(v.toString(), "1 2 3");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string(""));

        ASSERT_EQ(v.toString(), "");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string(" "));

        ASSERT_EQ(v.toString(), " ");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("Infinity"));
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("-Infinity"));
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("NaN"));
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }
}

TEST(ValueTest, CStringConstructor)
{
    {
        Value v("test");
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isInt());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v("Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v("-Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v("NaN");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }
}

TEST(ValueTest, CopyConstructor)
{
    {
        Value v1(50);
        Value v2(v1);
        ASSERT_EQ(v1.toInt(), v2.toInt());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_TRUE(v2.isNumber());
        ASSERT_TRUE(v2.isValidNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(5.5);
        Value v2(v1);
        ASSERT_EQ(v1.toDouble(), v2.toDouble());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_TRUE(v2.isNumber());
        ASSERT_TRUE(v2.isValidNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(true);
        Value v2(v1);
        ASSERT_EQ(v1.toBool(), v2.toBool());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_TRUE(v2.isValidNumber());
        ASSERT_TRUE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1("test");
        Value v2(v1);
        ASSERT_EQ(v1.toString(), v2.toString());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isValidNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_TRUE(v2.isString());
    }
}

TEST(ValueTest, DoubleAssignment)
{
    Value v;
    v = static_cast<double>(3.14);
    ASSERT_EQ(v.toDouble(), 3.14);
    ASSERT_EQ(v.type(), ValueType::Number);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_TRUE(v.isValidNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());

    v = std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.type(), ValueType::Number);
    ASSERT_TRUE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_TRUE(v.isValidNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());

    v = -std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.type(), ValueType::Number);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_TRUE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_TRUE(v.isValidNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());

    v = -std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(v.type(), ValueType::Number);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_TRUE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isValidNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, BoolAssignment)
{
    {
        Value v;
        v = true;
        ASSERT_EQ(v.toBool(), true);
        ASSERT_EQ(v.type(), ValueType::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_TRUE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v;
        v = false;
        ASSERT_EQ(v.toBool(), false);
        ASSERT_EQ(v.type(), ValueType::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_TRUE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, StdStringAssignment)
{
    {
        Value v;
        v = std::string("test");
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = std::string("Infinity");
        ASSERT_EQ(v.toString(), "Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = std::string("-Infinity");
        ASSERT_EQ(v.toString(), "-Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = std::string("NaN");
        ASSERT_EQ(v.toString(), "NaN");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }
}

TEST(ValueTest, CStringAssignment)
{
    {
        Value v;
        v = "test";
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = "Infinity";
        ASSERT_EQ(v.toString(), "Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = "-Infinity";
        ASSERT_EQ(v.toString(), "-Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = "NaN";
        ASSERT_EQ(v.toString(), "NaN");
        ASSERT_EQ(v.type(), ValueType::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isValidNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }
}

TEST(ValueTest, CopyAssignment)
{
    {
        Value v1(50);
        Value v2;
        v2 = v1;
        ASSERT_EQ(v2.toInt(), v1.toInt());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_TRUE(v2.isNumber());
        ASSERT_TRUE(v2.isValidNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(5.5);
        Value v2;
        v2 = v1;
        ASSERT_EQ(v1.toDouble(), v2.toDouble());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_TRUE(v2.isNumber());
        ASSERT_TRUE(v2.isValidNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(true);
        Value v2;
        v2 = v1;
        ASSERT_EQ(v1.toBool(), v2.toBool());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_TRUE(v2.isValidNumber());
        ASSERT_TRUE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1("test");
        Value v2;
        v2 = v1;
        ASSERT_EQ(v1.toString(), v2.toString());
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isValidNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_TRUE(v2.isString());
    }
}

TEST(ValueTest, ToInt)
{
    Value v = 50;
    ASSERT_EQ(v.toInt(), 50);
    v = -50;
    ASSERT_EQ(v.toInt(), -50);

    v = 2.54;
    ASSERT_EQ(v.toInt(), 2);

    v = -2.54;
    ASSERT_EQ(v.toInt(), -2);

    v = false;
    ASSERT_EQ(v.toInt(), 0);
    v = true;
    ASSERT_EQ(v.toInt(), 1);

    v = "255";
    ASSERT_EQ(v.toInt(), 255);
    v = "-255";
    ASSERT_EQ(v.toInt(), -255);

    v = "255.625";
    ASSERT_EQ(v.toInt(), 255);
    v = "-255.625";
    ASSERT_EQ(v.toInt(), -255);

    v = "false";
    ASSERT_EQ(v.toInt(), 0);
    v = "true";
    ASSERT_EQ(v.toInt(), 0); // booleans represented as string shouldn't convert

    v = "Infinity";
    ASSERT_EQ(v.toInt(), 0);
    v = "-Infinity";
    ASSERT_EQ(v.toInt(), 0);
    v = "NaN";
    ASSERT_EQ(v.toInt(), 0);

    v = std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toInt(), 0);
    v = -std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toInt(), 0);
    v = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(v.toInt(), 0);

    v = "something";
    ASSERT_EQ(v.toInt(), 0);

    // Hex
    v = "0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 2814);

    v = "   0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 2814);

    v = "0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 2814);

    v = "   0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 2814);

    v = "0x0afe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 2814);

    v = "0xBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 47821);

    v = "0XBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 47821);

    v = "0xAbG";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 0);

    v = "0xabf.d";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 0);

    // Octal
    v = "0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 326);

    v = "   0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 326);

    v = "0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 326);

    v = "   0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 326);

    v = "0o0506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 326);

    v = "0O17206";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 7814);

    v = "0o5783";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 0);

    v = "0o573.2";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 0);

    // Binary
    v = "0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 45);

    v = "   0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 45);

    v = "0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 45);

    v = "   0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 45);

    v = "0b0101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 45);

    v = "0B1110100110";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 934);

    v = "0b100112001";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 0);

    v = "0b10011001.1";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toInt(), 0);
}

TEST(ValueTest, ToLong)
{
    Value v = 123L;
    ASSERT_EQ(v.toLong(), 123L);
    v = -123L;
    ASSERT_EQ(v.toLong(), -123L);

    v = 2.54;
    ASSERT_EQ(v.toLong(), 2);

    v = -2.54;
    ASSERT_EQ(v.toLong(), -2);

    v = false;
    ASSERT_EQ(v.toLong(), 0);
    v = true;
    ASSERT_EQ(v.toLong(), 1);

    v = "255.625";
    ASSERT_EQ(v.toLong(), 255);
    v = "-255.625";
    ASSERT_EQ(v.toLong(), -255);

    v = "false";
    ASSERT_EQ(v.toLong(), 0);
    v = "true";
    ASSERT_EQ(v.toLong(), 0); // booleans represented as string shouldn't convert

    v = "Infinity";
    ASSERT_EQ(v.toLong(), 0);
    v = "-Infinity";
    ASSERT_EQ(v.toLong(), 0);
    v = "NaN";
    ASSERT_EQ(v.toLong(), 0);

    v = std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toLong(), 0);
    v = -std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toLong(), 0);
    v = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(v.toLong(), 0);

    v = "something";
    ASSERT_EQ(v.toLong(), 0);

    // Hex
    v = "0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 2814);

    v = "   0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 2814);

    v = "0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 2814);

    v = "   0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 2814);

    v = "0x0afe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 2814);

    v = "0xBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 47821);

    v = "0XBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 47821);

    v = "0xAbG";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 0);

    v = "0xabf.d";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 0);

    // Octal
    v = "0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 326);

    v = "   0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 326);

    v = "0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 326);

    v = "   0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 326);

    v = "0o0506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 326);

    v = "0O17206";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 7814);

    v = "0o5783";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 0);

    v = "0o573.2";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 0);

    // Binary
    v = "0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 45);

    v = "   0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 45);

    v = "0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 45);

    v = "   0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 45);

    v = "0b0101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 45);

    v = "0B1110100110";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 934);

    v = "0b100112001";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 0);

    v = "0b100112001.1";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toLong(), 0);
}

TEST(ValueTest, ToDouble)
{
    std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
    std::setlocale(LC_NUMERIC, "sk_SK.UTF-8");

    Value v = 2147483647;
    ASSERT_EQ(v.toDouble(), 2147483647.0);
    v = -2147483647;
    ASSERT_EQ(v.toDouble(), -2147483647.0);

    v = 2.54;
    ASSERT_EQ(v.toDouble(), 2.54);

    v = -2.54;
    ASSERT_EQ(v.toDouble(), -2.54);

    v = false;
    ASSERT_EQ(v.toDouble(), 0.0);
    v = true;
    ASSERT_EQ(v.toDouble(), 1.0);

    v = "2147483647";
    ASSERT_EQ(v.toDouble(), 2147483647.0);
    v = "-2147483647";
    ASSERT_EQ(v.toDouble(), -2147483647.0);

    v = "255.625";
    ASSERT_EQ(v.toDouble(), 255.625);
    v = "-255.625";
    ASSERT_EQ(v.toDouble(), -255.625);

    v = "0.15";
    ASSERT_EQ(v.toDouble(), 0.15);
    v = "-0.15";
    ASSERT_EQ(v.toDouble(), -0.15);

    v = "0";
    ASSERT_EQ(v.toDouble(), 0.0);
    v = "-0";
    ASSERT_EQ(v.toDouble(), -0.0);
    v = "0.0";
    ASSERT_EQ(v.toDouble(), 0.0);
    v = "-0.0";
    ASSERT_EQ(v.toDouble(), -0.0);

    v = "+.15";
    ASSERT_EQ(v.toDouble(), 0.15);
    v = ".15";
    ASSERT_EQ(v.toDouble(), 0.15);
    v = "-.15";
    ASSERT_EQ(v.toDouble(), -0.15);

    v = "1.";
    ASSERT_EQ(v.toDouble(), 1);
    v = "+1.";
    ASSERT_EQ(v.toDouble(), 1);
    v = "-1.";
    ASSERT_EQ(v.toDouble(), -1);

    v = "0+5";
    ASSERT_EQ(v.toDouble(), 0);
    v = "0-5";
    ASSERT_EQ(v.toDouble(), 0);

    v = "9432.4e-12";
    ASSERT_EQ(v.toDouble(), 9.4324e-9);
    v = "-9432.4e-12";
    ASSERT_EQ(v.toDouble(), -9.4324e-9);

    v = "9432.4e+6";
    ASSERT_EQ(v.toDouble(), 9.4324e+9);
    v = "-9432.4e+6";
    ASSERT_EQ(v.toDouble(), -9.4324e+9);

    v = "1 2 3";
    ASSERT_EQ(v.toDouble(), 0);

    v = "false";
    ASSERT_EQ(v.toDouble(), 0.0);
    v = "true";
    ASSERT_EQ(v.toDouble(), 0.0); // booleans represented as string shouldn't convert

    v = "Infinity";
    ASSERT_TRUE(v.toDouble() > 0 && std::isinf(v.toDouble()));
    v = "-Infinity";
    ASSERT_TRUE(v.toDouble() < 0 && std::isinf(v.toDouble()));
    v = "NaN";
    ASSERT_EQ(v.toDouble(), 0.0);
    v = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(v.toDouble(), 0.0);

    v = "something";
    ASSERT_EQ(v.toDouble(), 0.0);

    // Hex
    v = "0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 2814);

    v = "   0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 2814);

    v = "0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 2814);

    v = "   0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 2814);

    v = "0x0afe";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 2814);

    v = "0xBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 47821);

    v = "0XBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 47821);

    v = "0xAbG";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0xabf.d";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "+0xa";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "-0xa";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0x+a";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0x-a";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    // Octal
    v = "0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 326);

    v = "   0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 326);

    v = "0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 326);

    v = "   0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 326);

    v = "0o0506";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 326);

    v = "0O17206";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 7814);

    v = "0o5783";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0o573.2";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "+0o2";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "-0o2";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0o+2";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0o-2";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    // Binary
    v = "0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 45);

    v = "   0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 45);

    v = "0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 45);

    v = "   0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 45);

    v = "0b0101101";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 45);

    v = "0B1110100110";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 934);

    v = "0b100112001";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0b10011001.1";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "+0b1";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "-0b1";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0b+1";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    v = "0b-1";
    ASSERT_TRUE(v.isString());
    ASSERT_EQ(v.toDouble(), 0);

    std::setlocale(LC_NUMERIC, oldLocale.c_str());
}

TEST(ValueTest, ToBool)
{
    Value v = 2147483647;
    ASSERT_EQ(v.toBool(), true);
    v = -2147483647;
    ASSERT_EQ(v.toBool(), true);

    v = 2.54;
    ASSERT_EQ(v.toBool(), true);

    v = -2.54;
    ASSERT_EQ(v.toBool(), true);

    v = std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toBool(), true);

    v = -std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toBool(), true);

    v = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(v.toBool(), false);

    v = false;
    ASSERT_EQ(v.toBool(), false);
    v = true;
    ASSERT_EQ(v.toBool(), true);

    v = 0;
    ASSERT_EQ(v.toBool(), false);
    v = 1;
    ASSERT_EQ(v.toBool(), true);
    v = -1;
    ASSERT_EQ(v.toBool(), true);

    v = "2147483647";
    ASSERT_EQ(v.toBool(), true);
    v = "-2147483647";
    ASSERT_EQ(v.toBool(), true);

    v = "255.625";
    ASSERT_EQ(v.toBool(), true);
    v = "-255.625";
    ASSERT_EQ(v.toBool(), true);

    v = "0";
    ASSERT_EQ(v.toBool(), false);

    v = "false";
    ASSERT_EQ(v.toBool(), false);
    v = "true";
    ASSERT_EQ(v.toBool(), true);
    v = "FaLsE";
    ASSERT_EQ(v.toBool(), false);
    v = "TrUe";
    ASSERT_EQ(v.toBool(), true);

    v = "Infinity";
    ASSERT_EQ(v.toBool(), true);
    v = "-Infinity";
    ASSERT_EQ(v.toBool(), true);
    v = "NaN";
    ASSERT_EQ(v.toBool(), true);

    v = "something";
    ASSERT_EQ(v.toBool(), true);
    v = "";
    ASSERT_EQ(v.toBool(), false);

    // Hex
    v = "0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "   0xafe";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "   0xafe   ";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0x0afe";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0xBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0x0";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0XBaCD";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0xAbG";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0xabf.d";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    // Octal
    v = "0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "   0o506";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "   0o506   ";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0o0506";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0O17206";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0o5783";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0o573.2";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    // Binary
    v = "0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "   0b101101";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "   0b101101   ";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0b0101101";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0B1110100110";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0b100112001";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());

    v = "0b10011001.1";
    ASSERT_TRUE(v.isString());
    ASSERT_TRUE(v.toBool());
}

TEST(ValueTest, ToString)
{
    std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
    std::setlocale(LC_NUMERIC, "sk_SK.UTF-8");

    Value v = 2147483647;
    ASSERT_EQ(v.toString(), "2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = -2147483647;
    ASSERT_EQ(v.toString(), "-2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 512L;
    ASSERT_EQ(v.toString(), "512");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = -512L;
    ASSERT_EQ(v.toString(), "-512");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 0.0;
    ASSERT_EQ(v.toString(), "0");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -0.0;
    ASSERT_EQ(v.toString(), "0");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 2.0;
    ASSERT_EQ(v.toString(), "2");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -2.0;
    ASSERT_EQ(v.toString(), "-2");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 2.54;
    ASSERT_EQ(v.toString(), "2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -2.54;
    ASSERT_EQ(v.toString(), "-2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 59.8;
    ASSERT_EQ(v.toString(), "59.8");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -59.8;
    ASSERT_EQ(v.toString(), "-59.8");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 5.3;
    ASSERT_EQ(v.toString(), "5.3");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -5.3;
    ASSERT_EQ(v.toString(), "-5.3");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 2550.625021000115;
    ASSERT_EQ(v.toString(), "2550.625021000115");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -2550.625021000115;
    ASSERT_EQ(v.toString(), "-2550.625021000115");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 9.4324e+20;
    ASSERT_EQ(v.toString(), "943240000000000000000");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -2.591e-2;
    ASSERT_EQ(v.toString(), "-0.02591");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 9.4324e+21;
    ASSERT_EQ(v.toString(), "9.4324e+21");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = -2.591e-13;
    ASSERT_EQ(v.toString(), "-2.591e-13");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 0.001;
    ASSERT_EQ(v.toString(), "0.001");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = -0.001;
    ASSERT_EQ(v.toString(), "-0.001");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 0.000001;
    ASSERT_EQ(v.toString(), "0.000001");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = -0.000001;
    ASSERT_EQ(v.toString(), "-0.000001");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = 0.0000001;
    ASSERT_EQ(v.toString(), "1e-7");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = -0.0000001;
    ASSERT_EQ(v.toString(), "-1e-7");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = false;
    ASSERT_EQ(v.toString(), "false");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = true;
    ASSERT_EQ(v.toString(), "true");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "2147483647";
    ASSERT_EQ(v.toString(), "2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "-2147483647";
    ASSERT_EQ(v.toString(), "-2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "999999999999999999";
    ASSERT_EQ(v.toString(), "999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "-999999999999999999";
    ASSERT_EQ(v.toString(), "-999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "255.625";
    ASSERT_EQ(v.toString(), "255.625");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "-255.625";
    ASSERT_EQ(v.toString(), "-255.625");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "9432.4e-12";
    ASSERT_EQ(v.toString(), "9432.4e-12");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "-9432.4e-12";
    ASSERT_EQ(v.toString(), "-9432.4e-12");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "9432.4e+6";
    ASSERT_EQ(v.toString(), "9432.4e+6");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "-9432.4e+6";
    ASSERT_EQ(v.toString(), "-9432.4e+6");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "false";
    ASSERT_EQ(v.toString(), "false");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "true";
    ASSERT_EQ(v.toString(), "true");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "Infinity";
    ASSERT_TRUE(v.isInfinity());
    ASSERT_EQ(v.toString(), "Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "-Infinity";
    ASSERT_TRUE(v.isNegativeInfinity());
    ASSERT_EQ(v.toString(), "-Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = "NaN";
    ASSERT_TRUE(v.isNaN());
    ASSERT_EQ(v.toString(), "NaN");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = "something";
    ASSERT_EQ(v.toString(), "something");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    v = std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toString(), "Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = -std::numeric_limits<double>::infinity();
    ASSERT_EQ(v.toString(), "-Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());
    v = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(v.toString(), "NaN");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::u16string(value_toStringPtr(&v.data())->data), v.toUtf16());

    std::setlocale(LC_NUMERIC, oldLocale.c_str());
}

TEST(ValueTest, ToRgba)
{
    Value v = "#ff0000";
    ASSERT_EQ(v.toRgba(), rgba(255, 0, 0, 255));

    v = "#00ff00";
    ASSERT_EQ(v.toRgba(), rgba(0, 255, 0, 255));

    v = "#0000ff";
    ASSERT_EQ(v.toRgba(), rgba(0, 0, 255, 255));

    v = "#800000";
    ASSERT_EQ(v.toRgba(), rgba(128, 0, 0, 255));

    v = "#008000";
    ASSERT_EQ(v.toRgba(), rgba(0, 128, 0, 255));

    v = "#000080";
    ASSERT_EQ(v.toRgba(), rgba(0, 0, 128, 255));

    v = "#FF0080FF";
    ASSERT_EQ(v.toRgba(), rgba(0, 0, 0, 255));

    v = "#abc";
    ASSERT_EQ(v.toRgba(), rgba(170, 187, 204, 255));

    v = "00ff00";
    ASSERT_EQ(v.toRgba(), 0);

    v = "123";
    ASSERT_EQ(v.toRgba(), 123);

    v = "#12345";
    ASSERT_EQ(v.toRgba(), rgba(0, 0, 0, 255));

    v = "";
    ASSERT_EQ(v.toRgba(), rgba(0, 0, 0, 255));

    v = "#gggggg";
    ASSERT_EQ(v.toRgba(), rgba(0, 0, 0, 255));

    v = 2164195583;
    ASSERT_EQ(v.toRgba(), 2164195583);

    v = 0;
    ASSERT_EQ(v.toRgba(), 0);

    v = true;
    ASSERT_EQ(v.toRgba(), 1);

    v = false;
    ASSERT_EQ(v.toRgba(), 0);
}

TEST(ValueTest, AddFunction)
{
    Value v = 50;
    v.add(25);
    ASSERT_EQ(v.toInt(), 75);

    v = 999999999999999999L;
    v.add(1);
    ASSERT_EQ(v.toLong(), 1000000000000000000L);

    v = -500;
    v.add(25);
    ASSERT_EQ(v.toInt(), -475);
    v.add(-25);
    ASSERT_EQ(v.toInt(), -500);

    v = 2.54;
    v.add(6.28);
    ASSERT_EQ(v.toDouble(), 8.82);
    v.add(-6.28);
    ASSERT_EQ(v.toDouble(), 2.54);

    v = "2.54";
    v.add("6.28");
    ASSERT_EQ(v.toDouble(), 8.82);
    v.add("-6.28");
    ASSERT_EQ(v.toDouble(), 2.54);

    v = true;
    v.add(true);
    ASSERT_EQ(v.toInt(), 2);

    v = "Infinity";
    v.add("Infinity");
    ASSERT_TRUE(v.isInfinity());

    v = "Infinity";
    v.add("-Infinity");
    ASSERT_TRUE(v.isNaN());

    v = "-Infinity";
    v.add("Infinity");
    ASSERT_TRUE(v.isNaN());

    v = "-Infinity";
    v.add("-Infinity");
    ASSERT_TRUE(v.isNegativeInfinity());

    v = 1;
    v.add("NaN");
    ASSERT_EQ(v.toInt(), 1);
}

TEST(ValueTest, AddOperator)
{
    Value v = 50;
    ASSERT_EQ(v + 25, 75);

    v = 999999999999999999L;
    ASSERT_EQ(v + 1, 1000000000000000000L);

    v = -500;
    ASSERT_EQ(v + 25, -475);
    ASSERT_EQ(v + (-25), -525);

    v = 2.54;
    ASSERT_EQ(v + 6.28, 8.82);
    ASSERT_EQ(v + (-6.28), -3.74);

    v = "2.54";
    ASSERT_EQ(v + "6.28", 8.82);
    ASSERT_EQ(v + "-6.28", -3.74);

    v = true;
    ASSERT_EQ(v + true, 2);

    v = "Infinity";
    ASSERT_TRUE((v + "Infinity").isInfinity());

    v = "Infinity";
    ASSERT_TRUE((v + "-Infinity").isNaN());

    v = "-Infinity";
    ASSERT_TRUE((v + "Infinity").isNaN());

    v = "-Infinity";
    ASSERT_TRUE((v + "-Infinity").isNegativeInfinity());

    v = 1;
    ASSERT_EQ(v + "NaN", 1);
}

TEST(ValueTest, SubtractFunction)
{
    Value v = 50;
    v.subtract(25);
    ASSERT_EQ(v.toInt(), 25);

    v = 256L;
    v.subtract(1);
    ASSERT_EQ(v.toLong(), 255L);

    v = -500;
    v.subtract(25);
    ASSERT_EQ(v.toInt(), -525);
    v.subtract(-25);
    ASSERT_EQ(v.toInt(), -500);

    v = 2.54;
    v.subtract(6.28);
    ASSERT_EQ(v.toDouble(), -3.74);
    v.subtract(-6.28);
    ASSERT_EQ(v.toDouble(), 2.54);

    v = "2.54";
    v.subtract("6.28");
    ASSERT_EQ(v.toDouble(), -3.74);
    v.subtract("-6.28");
    ASSERT_EQ(v.toDouble(), 2.54);

    v = true;
    v.subtract(true);
    ASSERT_EQ(v.toInt(), 0);

    v = "Infinity";
    v.subtract("Infinity");
    ASSERT_TRUE(v.isNaN());

    v = "Infinity";
    v.subtract("-Infinity");
    ASSERT_TRUE(v.isInfinity());

    v = "-Infinity";
    v.subtract("Infinity");
    ASSERT_TRUE(v.isNegativeInfinity());

    v = "-Infinity";
    v.subtract("-Infinity");
    ASSERT_TRUE(v.isNaN());

    v = 1;
    v.subtract("NaN");
    ASSERT_EQ(v.toInt(), 1);
}

TEST(ValueTest, SubtractOperator)
{
    Value v = 50;
    ASSERT_EQ(v - 25, 25);

    v = 999999999999999999L;
    ASSERT_EQ(v - 1, 999999999999999998L);

    v = -500;
    ASSERT_EQ(v - 25, -525);
    ASSERT_EQ(v - (-25), -475);

    v = 2.54;
    ASSERT_EQ(v - 6.28, -3.74);
    ASSERT_EQ(v - (-6.28), 8.82);

    v = "2.54";
    ASSERT_EQ(v - "6.28", -3.74);
    ASSERT_EQ(v - "-6.28", 8.82);

    v = true;
    ASSERT_EQ(v - true, 0);

    v = "Infinity";
    ASSERT_TRUE((v - "Infinity").isNaN());

    v = "Infinity";
    ASSERT_TRUE((v - "-Infinity").isInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v - "Infinity").isNegativeInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v - "-Infinity").isNaN());

    v = 1;
    ASSERT_EQ(v - "NaN", 1);
}

TEST(ValueTest, MultiplyFunction)
{
    Value v = 50;
    v.multiply(2);
    ASSERT_EQ(v.toInt(), 100);

    v = 128L;
    v.multiply(2);
    ASSERT_EQ(v.toLong(), 256L);

    v = -500;
    v.multiply(25);
    ASSERT_EQ(v.toInt(), -12500);
    v.multiply(-25);
    ASSERT_EQ(v.toInt(), 312500);

    v = 2.54;
    v.multiply(6.28);
    ASSERT_EQ(v.toDouble(), 15.9512);

    v = "2.54";
    v.multiply("6.28");
    ASSERT_EQ(v.toDouble(), 15.9512);

    v = true;
    v.multiply(true);
    ASSERT_EQ(v.toInt(), 1);

    v = "Infinity";
    v.multiply("Infinity");
    ASSERT_TRUE(v.isInfinity());

    v = "Infinity";
    v.multiply(0);
    ASSERT_TRUE(v.isNaN());

    v = "Infinity";
    v.multiply(2);
    ASSERT_TRUE(v.isInfinity());

    v = "Infinity";
    v.multiply("-Infinity");
    ASSERT_TRUE(v.isNegativeInfinity());

    v = "-Infinity";
    v.multiply("Infinity");
    ASSERT_TRUE(v.isNegativeInfinity());

    v = "-Infinity";
    v.multiply(0);
    ASSERT_TRUE(v.isNaN());

    v = "-Infinity";
    v.multiply(2);
    ASSERT_TRUE(v.isNegativeInfinity());

    v = "-Infinity";
    v.multiply("-Infinity");
    ASSERT_TRUE(v.isInfinity());

    v = 0;
    v.multiply("Infinity");
    ASSERT_TRUE(v.isNaN());

    v = 2;
    v.multiply("Infinity");
    ASSERT_TRUE(v.isInfinity());

    v = 0;
    v.multiply("-Infinity");
    ASSERT_TRUE(v.isNaN());

    v = 2;
    v.multiply("-Infinity");
    ASSERT_TRUE(v.isNegativeInfinity());

    v = 1;
    v.multiply("NaN");
    ASSERT_EQ(v.toInt(), 0);
}

TEST(ValueTest, MultiplyOperator)
{
    Value v = 50;
    ASSERT_EQ(v * 2, 100);

    v = 999999999999999999L;
    ASSERT_EQ(v * 2, 1999999999999999998L);

    v = -500;
    ASSERT_EQ(v * 25, -12500);
    ASSERT_EQ(v * (-25), 12500);

    v = 2.54;
    ASSERT_EQ(v * 6.28, 15.9512);

    v = "2.54";
    ASSERT_EQ(v * "6.28", 15.9512);

    v = true;
    ASSERT_EQ(v * true, 1);

    v = "Infinity";
    ASSERT_TRUE((v * "Infinity").isInfinity());

    v = "Infinity";
    ASSERT_TRUE((v * 0).isNaN());

    v = "Infinity";
    ASSERT_TRUE((v * 2).isInfinity());

    v = "Infinity";
    ASSERT_TRUE((v * "-Infinity").isNegativeInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v * "Infinity").isNegativeInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v * 0).isNaN());

    v = "-Infinity";
    ASSERT_TRUE((v * 2).isNegativeInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v * "-Infinity").isInfinity());

    v = 0;
    ASSERT_TRUE((v * "Infinity").isNaN());

    v = 2;
    ASSERT_TRUE((v * "Infinity").isInfinity());

    v = 0;
    ASSERT_TRUE((v * "-Infinity").isNaN());

    v = 2;
    ASSERT_TRUE((v * "-Infinity").isNegativeInfinity());

    v = 1;
    ASSERT_EQ((v * "NaN"), 0);
}

TEST(ValueTest, DivideFunction)
{
    Value v = 50;
    v.divide(2);
    ASSERT_EQ(v.toInt(), 25);

    v = -500;
    v.divide(25);
    ASSERT_EQ(v.toInt(), -20);
    v.divide(-25);
    ASSERT_EQ(v.toDouble(), 0.8);

    v = 3.5;
    v.divide(2.5);
    ASSERT_EQ(v.toDouble(), 1.4);

    v = "3.5";
    v.divide("2.5");
    ASSERT_EQ(v.toDouble(), 1.4);

    v = true;
    v.divide(true);
    ASSERT_EQ(v.toInt(), 1);

    v = "Infinity";
    v.divide("Infinity");
    ASSERT_TRUE(v.isNaN());

    v = "Infinity";
    v.divide(0);
    ASSERT_TRUE(v.isInfinity());

    v = "Infinity";
    v.divide(2);
    ASSERT_TRUE(v.isInfinity());

    v = "Infinity";
    v.divide(-2);
    ASSERT_TRUE(v.isNegativeInfinity());

    v = "Infinity";
    v.divide("-Infinity");
    ASSERT_TRUE(v.isNaN());

    v = "-Infinity";
    v.divide("Infinity");
    ASSERT_TRUE(v.isNaN());

    v = "-Infinity";
    v.divide(0);
    ASSERT_TRUE(v.isNegativeInfinity());

    v = "-Infinity";
    v.divide(2);
    ASSERT_TRUE(v.isNegativeInfinity());

    v = "-Infinity";
    v.divide(-2);
    ASSERT_TRUE(v.isInfinity());

    v = "-Infinity";
    v.divide("-Infinity");
    ASSERT_TRUE(v.isNaN());

    v = 0;
    v.divide("Infinity");
    ASSERT_FALSE(v.isNaN());
    ASSERT_EQ(v.toDouble(), 0.0);

    v = 2;
    v.divide("Infinity");
    ASSERT_FALSE(v.isNaN());
    ASSERT_EQ(v.toDouble(), 0.0);

    v = -2;
    v.divide("Infinity");
    ASSERT_FALSE(v.isNaN());
    ASSERT_EQ(v.toDouble(), 0.0);

    v = 0;
    v.divide("-Infinity");
    ASSERT_FALSE(v.isNaN());
    ASSERT_EQ(v.toDouble(), 0.0);

    v = 2;
    v.divide("-Infinity");
    ASSERT_FALSE(v.isNaN());
    ASSERT_EQ(v.toDouble(), 0.0);

    v = -2;
    v.divide("-Infinity");
    ASSERT_FALSE(v.isNaN());
    ASSERT_EQ(v.toDouble(), 0.0);

    v = 1;
    v.divide("NaN");
    ASSERT_TRUE(v.isInfinity());

    v = 5;
    v.divide(0);
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isInfinity());

    v = -5;
    v.divide(0);
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNegativeInfinity());

    v = 0;
    v.divide(0);
    ASSERT_TRUE(v.isNaN());
}

TEST(ValueTest, DivideOperator)
{
    Value v = 50;
    ASSERT_EQ(v / 2, 25);

    v = -500;
    ASSERT_EQ(v / 25, -20);
    ASSERT_EQ(v / (-25), 20);

    v = 3.5;
    ASSERT_EQ(v / 2.5, 1.4);

    v = "3.5";
    ASSERT_EQ(v / "2.5", 1.4);

    v = true;
    ASSERT_EQ(v / true, 1);

    v = "Infinity";
    ASSERT_TRUE((v / "Infinity").isNaN());

    v = "Infinity";
    ASSERT_TRUE((v / 0).isInfinity());

    v = "Infinity";
    ASSERT_TRUE((v / 2).isInfinity());

    v = "Infinity";
    ASSERT_TRUE((v / -2).isNegativeInfinity());

    v = "Infinity";
    ASSERT_TRUE((v / "-Infinity").isNaN());

    v = "-Infinity";
    ASSERT_TRUE((v / "Infinity").isNaN());

    v = "-Infinity";
    ASSERT_TRUE((v / 0).isNegativeInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v / 2).isNegativeInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v / -2).isInfinity());

    v = "-Infinity";
    ASSERT_TRUE((v / "-Infinity").isNaN());

    v = 0;
    ASSERT_EQ(v / "Infinity", 0.0);

    v = 2;
    ASSERT_EQ(v / "Infinity", 0.0);

    v = -2;
    ASSERT_EQ(v / "Infinity", 0.0);

    v = 0;
    ASSERT_EQ(v / "-Infinity", 0.0);

    v = 2;
    ASSERT_EQ(v / "-Infinity", 0.0);

    v = -2;
    ASSERT_EQ(v / "-Infinity", 0.0);

    v = 1;
    ASSERT_TRUE((v / "NaN").isInfinity());

    v = 5;
    ASSERT_FALSE((v / 0).isNaN());
    ASSERT_TRUE((v / 0).isInfinity());

    v = -5;
    ASSERT_FALSE((v / 0).isNaN());
    ASSERT_TRUE((v / 0).isNegativeInfinity());

    v = 0;
    ASSERT_TRUE((v / 0).isNaN());
}

TEST(ValueTest, ModFunction)
{
    Value v = 5;
    v.mod(2);
    ASSERT_EQ(v.toInt(), 1);

    v = 4;
    v.mod(2);
    ASSERT_EQ(v.toInt(), 0);

    v = 11;
    v.mod(3);
    ASSERT_EQ(v.toInt(), 2);

    v = 50.543;
    v.mod(7);
    ASSERT_EQ(v.toDouble(), 1.5429999999999993);

    v = -50.543;
    v.mod(7);
    ASSERT_EQ(v.toDouble(), 5.4570000000000007);

    v = -11;
    v.mod(3);
    ASSERT_EQ(v.toInt(), 1);

    v = -3;
    v.mod(3);
    ASSERT_EQ(v.toInt(), 0);

    v = 10;
    v.mod(-3);
    ASSERT_EQ(v.toInt(), -2);

    v = 3;
    v.mod(-3);
    ASSERT_EQ(v.toInt(), 0);

    v = -5;
    v.mod(-3);
    ASSERT_EQ(v.toInt(), -2);

    v = -5;
    v.mod(-5);
    ASSERT_EQ(v.toInt(), 0);

    v = 3;
    v.mod(0);
    ASSERT_TRUE(v.isNaN());

    v = -3;
    v.mod(0);
    ASSERT_TRUE(v.isNaN());

    v = 0;
    v.mod(0);
    ASSERT_TRUE(v.isNaN());

    v = 150;
    v.mod("Infinity");
    ASSERT_EQ(v.toInt(), 150);

    v = -150;
    v.mod("Infinity");
    ASSERT_EQ(v.toInt(), -150);

    v = 150;
    v.mod("-Infinity");
    ASSERT_EQ(v.toInt(), 150);

    v = -150;
    v.mod("-Infinity");
    ASSERT_EQ(v.toInt(), -150);
}

TEST(ValueTest, ModOperator)
{
    Value v = 5;
    ASSERT_EQ(v % 2, 1);

    v = 4;
    ASSERT_EQ(v % 2, 0);

    v = 11;
    ASSERT_EQ(v % 3, 2);

    v = 50.543;
    ASSERT_EQ(v % 7, 1.5429999999999993);

    v = -50.543;
    ASSERT_EQ(v % 7, 5.4570000000000007);

    v = -11;
    ASSERT_EQ(v % 3, 1);

    v = -3;
    ASSERT_EQ(v % 3, 0);

    v = 10;
    ASSERT_EQ(v % (-3), -2);

    v = 3;
    ASSERT_EQ(v % (-3), 0);

    v = -5;
    ASSERT_EQ(v % (-3), -2);

    v = -5;
    ASSERT_EQ(v % (-5), 0);

    v = 3;
    ASSERT_TRUE((v % 0).isNaN());

    v = -3;
    ASSERT_TRUE((v % 0).isNaN());

    v = 0;
    ASSERT_TRUE((v % 0).isNaN());

    v = 150;
    ASSERT_EQ(v % "Infinity", 150);

    v = -150;
    ASSERT_EQ(v % "Infinity", -150);

    v = 150;
    ASSERT_EQ(v % "-Infinity", 150);

    v = -150;
    ASSERT_EQ(v % "-Infinity", -150);
}

TEST(ValueTest, ComparisonOperators)
{
    {
        Value v1 = 10;
        Value v2 = 10;
        Value v3 = 8;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_TRUE(v1 > v3);
        ASSERT_FALSE(v1 < v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_TRUE(v2 > v3);
        ASSERT_FALSE(v2 < v3);
    }

    {
        Value v1 = -4.25;
        Value v2 = -4.25;
        Value v3 = 5.312;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_FALSE(v1 > v3);
        ASSERT_TRUE(v1 < v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_FALSE(v2 > v3);
        ASSERT_TRUE(v2 < v3);
    }

    {
        Value v1 = true;
        Value v2 = true;
        Value v3 = false;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_TRUE(v1 > v3);
        ASSERT_FALSE(v1 < v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_TRUE(v2 > v3);
        ASSERT_FALSE(v2 < v3);
    }

    {
        Value v1 = "abC def";
        Value v2 = "abC def";
        Value v3 = "abc dEf";
        Value v4 = "hello world";

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_TRUE(v1 == v3);
        ASSERT_FALSE(v1 != v3);

        ASSERT_TRUE(v2 == v3);
        ASSERT_FALSE(v2 != v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);
        ASSERT_FALSE(v1 > v4);
        ASSERT_TRUE(v1 < v4);

        ASSERT_FALSE(v2 == v4);
        ASSERT_TRUE(v2 != v4);
        ASSERT_FALSE(v2 > v4);
        ASSERT_TRUE(v2 < v4);

        ASSERT_FALSE(v3 == v4);
        ASSERT_TRUE(v3 != v4);
        ASSERT_FALSE(v3 > v4);
        ASSERT_TRUE(v3 < v4);
    }

    {
        Value v1 = " ";
        Value v2 = "";
        Value v3 = "0";
        Value v4 = 0;

        ASSERT_FALSE(v1 == v2);
        ASSERT_TRUE(v1 != v2);
        ASSERT_TRUE(v1 > v2);
        ASSERT_FALSE(v1 < v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_FALSE(v1 > v3);
        ASSERT_TRUE(v1 < v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);
        ASSERT_FALSE(v1 > v4);
        ASSERT_TRUE(v1 < v4);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_FALSE(v2 > v3);
        ASSERT_TRUE(v2 < v3);

        ASSERT_FALSE(v2 == v4);
        ASSERT_TRUE(v2 != v4);
        ASSERT_FALSE(v2 > v4);
        ASSERT_TRUE(v2 < v4);

        ASSERT_TRUE(v3 == v4);
        ASSERT_FALSE(v3 != v4);
    }

    {
        Value v1(std::numeric_limits<double>::infinity());
        Value v2(std::numeric_limits<double>::infinity());
        Value v3(-std::numeric_limits<double>::infinity());
        Value v4(-std::numeric_limits<double>::infinity());
        Value v5(std::numeric_limits<double>::quiet_NaN());
        Value v6(std::numeric_limits<double>::quiet_NaN());

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_TRUE(v3 == v4);
        ASSERT_FALSE(v3 != v4);

        ASSERT_TRUE(v5 == v6);
        ASSERT_FALSE(v5 != v6);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_TRUE(v1 > v3);
        ASSERT_FALSE(v1 < v3);

        ASSERT_FALSE(v1 == v5);
        ASSERT_TRUE(v1 != v5);

        ASSERT_FALSE(v3 == v5);
        ASSERT_TRUE(v3 != v5);
    }

    {
        Value v1 = 5.00;
        Value v2 = 5;
        Value v3 = 6;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_FALSE(v1 > v3);
        ASSERT_TRUE(v1 < v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_FALSE(v2 > v3);
        ASSERT_TRUE(v2 < v3);
    }

    {
        Value v1 = 1;
        Value v2 = true;
        Value v3 = false;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_TRUE(v1 > v3);
        ASSERT_FALSE(v1 < v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_TRUE(v2 > v3);
        ASSERT_FALSE(v2 < v3);
    }

    {
        Value v1 = 5.25;
        Value v2 = "5.25";
        Value v3 = " 5.25";
        Value v4 = "5.25 ";
        Value v5 = " 5.25 ";
        Value v6 = "  5.25  ";
        Value v7 = "5.26";
        Value v8 = 5;
        Value v9 = "5  ";

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_TRUE(v1 == v3);
        ASSERT_FALSE(v1 != v3);

        ASSERT_TRUE(v1 == v4);
        ASSERT_FALSE(v1 != v4);

        ASSERT_TRUE(v1 == v5);
        ASSERT_FALSE(v1 != v5);

        ASSERT_TRUE(v1 == v6);
        ASSERT_FALSE(v1 != v6);

        ASSERT_FALSE(v1 == v7);
        ASSERT_TRUE(v1 != v7);
        ASSERT_FALSE(v1 > v7);
        ASSERT_TRUE(v1 < v7);

        ASSERT_FALSE(v2 == v7);
        ASSERT_TRUE(v2 != v7);
        ASSERT_FALSE(v2 > v7);
        ASSERT_TRUE(v2 < v7);

        ASSERT_FALSE(v3 == v7);
        ASSERT_TRUE(v3 != v7);

        ASSERT_FALSE(v4 == v7);
        ASSERT_TRUE(v4 != v7);

        ASSERT_FALSE(v5 == v7);
        ASSERT_TRUE(v5 != v7);

        ASSERT_FALSE(v6 == v7);
        ASSERT_TRUE(v6 != v7);

        ASSERT_TRUE(v8 == v9);
        ASSERT_FALSE(v8 != v9);
    }

    {
        Value v1 = 0;
        Value v2 = "0";
        Value v3 = "1";
        Value v4 = "test";

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_FALSE(v1 > v3);
        ASSERT_TRUE(v1 < v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);
    }

    {
        Value v1 = 1;
        Value v2 = true;
        Value v3 = false;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_TRUE(v1 > v3);
        ASSERT_FALSE(v1 < v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_TRUE(v2 > v3);
        ASSERT_FALSE(v2 < v3);
    }

    {
        Value v1 = 5;
        Value v2 = 0;
        Value v3(std::numeric_limits<double>::infinity());
        Value v4(-std::numeric_limits<double>::infinity());
        Value v5(std::numeric_limits<double>::quiet_NaN());

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_FALSE(v1 > v3);
        ASSERT_TRUE(v1 < v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);
        ASSERT_TRUE(v1 > v4);
        ASSERT_FALSE(v1 < v4);

        ASSERT_FALSE(v1 == v5);
        ASSERT_TRUE(v1 != v5);

        ASSERT_FALSE(v2 == v5);
        ASSERT_TRUE(v2 != v5);
        ASSERT_FALSE(v2 > v5);
        ASSERT_TRUE(v2 < v5);

        // NaN vs Infinity
        ASSERT_FALSE(v3 > v5);
        ASSERT_TRUE(v2 < v5);

        // NaN vs -Infinity
        ASSERT_FALSE(v4 > v5);
        ASSERT_TRUE(v2 < v5);
    }

    {
        Value v1 = true;
        Value v2 = false;
        Value v3 = "true";
        Value v4 = "false";
        Value v5 = "TRUE";
        Value v6 = "FALSE";

        ASSERT_TRUE(v1 == v3);
        ASSERT_FALSE(v1 != v3);
        ASSERT_FALSE(v1 > v3);
        ASSERT_FALSE(v1 < v3);

        ASSERT_TRUE(v2 == v4);
        ASSERT_FALSE(v2 != v4);
        ASSERT_FALSE(v2 > v4);
        ASSERT_FALSE(v2 < v4);

        ASSERT_FALSE(v1 == v2);
        ASSERT_TRUE(v1 != v2);
        ASSERT_TRUE(v1 > v2);
        ASSERT_FALSE(v1 < v2);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
        ASSERT_FALSE(v2 > v3);
        ASSERT_TRUE(v2 < v3);

        ASSERT_TRUE(v1 == v5);
        ASSERT_FALSE(v1 != v5);

        ASSERT_TRUE(v2 == v6);
        ASSERT_FALSE(v2 != v6);
    }

    {
        Value v1 = true;
        Value v2 = false;
        Value v3 = "00001";
        Value v4 = "00000";

        ASSERT_TRUE(v1 == v3);
        ASSERT_FALSE(v1 != v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);
        ASSERT_TRUE(v1 > v4);
        ASSERT_FALSE(v1 < v4);

        ASSERT_TRUE(v2 == v4);
        ASSERT_FALSE(v2 != v4);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = "true";
        Value v2 = "false";
        Value v3 = 1;
        Value v4 = 0;

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);

        ASSERT_FALSE(v2 == v4);
        ASSERT_TRUE(v2 != v4);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = "true";
        Value v2 = "false";
        Value v3 = "TRUE";
        Value v4 = "FALSE";

        ASSERT_TRUE(v1 == v3);
        ASSERT_FALSE(v1 != v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);

        ASSERT_TRUE(v2 == v4);
        ASSERT_FALSE(v2 != v4);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = true;
        Value v2 = false;
        Value v3(std::numeric_limits<double>::infinity());
        Value v4(-std::numeric_limits<double>::infinity());
        Value v5(std::numeric_limits<double>::quiet_NaN());

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);
        ASSERT_FALSE(v1 > v3);
        ASSERT_TRUE(v1 < v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);
        ASSERT_TRUE(v1 > v4);
        ASSERT_FALSE(v1 < v4);

        ASSERT_FALSE(v1 == v5);
        ASSERT_TRUE(v1 != v5);
        ASSERT_TRUE(v1 > v5);
        ASSERT_FALSE(v1 < v5);

        ASSERT_FALSE(v2 == v5);
        ASSERT_TRUE(v2 != v5);
        ASSERT_FALSE(v2 > v5);
        ASSERT_TRUE(v2 < v5);
    }

    {
        Value v1 = 0;
        Value v2(std::numeric_limits<double>::quiet_NaN());

        ASSERT_FALSE(v1 == v2);
        ASSERT_TRUE(v1 != v2);
        ASSERT_FALSE(v1 > v2);
        ASSERT_TRUE(v1 < v2);
    }

    {
        Value v1 = "Infinity";
        Value v2 = "infinity";
        Value v3 = "-Infinity";
        Value v4 = "-infinity";
        Value v5 = "NaN";
        Value v6 = "nan";
        Value v7(std::numeric_limits<double>::infinity());
        Value v8(-std::numeric_limits<double>::infinity());
        Value v9(std::numeric_limits<double>::quiet_NaN());

        // Infinity
        ASSERT_TRUE(v1 == v7);
        ASSERT_FALSE(v1 != v7);
        ASSERT_FALSE(v1 > v7);
        ASSERT_FALSE(v1 < v7);

        ASSERT_FALSE(v1 == v8);
        ASSERT_TRUE(v1 != v8);
        ASSERT_TRUE(v1 > v8);
        ASSERT_FALSE(v1 < v8);

        ASSERT_FALSE(v1 == v9);
        ASSERT_TRUE(v1 != v9);
        ASSERT_FALSE(v1 > v7);
        ASSERT_FALSE(v1 < v7);

        // infinity
        ASSERT_TRUE(v2 == v7);
        ASSERT_FALSE(v2 != v7);

        ASSERT_FALSE(v2 == v8);
        ASSERT_TRUE(v2 != v8);

        ASSERT_FALSE(v2 == v9);
        ASSERT_TRUE(v2 != v9);

        // -Infinity
        ASSERT_FALSE(v3 == v7);
        ASSERT_TRUE(v3 != v7);

        ASSERT_TRUE(v3 == v8);
        ASSERT_FALSE(v3 != v8);

        ASSERT_FALSE(v3 == v9);
        ASSERT_TRUE(v3 != v9);

        // -infinity
        ASSERT_FALSE(v4 == v7);
        ASSERT_TRUE(v4 != v7);

        ASSERT_TRUE(v4 == v8);
        ASSERT_FALSE(v4 != v8);

        ASSERT_FALSE(v4 == v9);
        ASSERT_TRUE(v4 != v9);

        // NaN
        ASSERT_FALSE(v5 == v7);
        ASSERT_TRUE(v5 != v7);

        ASSERT_FALSE(v5 == v8);
        ASSERT_TRUE(v5 != v8);

        ASSERT_TRUE(v5 == v9);
        ASSERT_FALSE(v5 != v9);

        // nan
        ASSERT_FALSE(v6 == v7);
        ASSERT_TRUE(v6 != v7);

        ASSERT_FALSE(v6 == v8);
        ASSERT_TRUE(v6 != v8);

        ASSERT_TRUE(v6 == v9);
        ASSERT_FALSE(v6 != v9);

        // NaN vs Infinity
        ASSERT_TRUE(v5 > v7);
        ASSERT_FALSE(v5 < v7);
        ASSERT_FALSE(v1 > v9);
        ASSERT_TRUE(v1 < v9);

        // NaN vs -Infinity
        ASSERT_TRUE(v5 > v8);
        ASSERT_FALSE(v5 < v8);
        ASSERT_FALSE(v3 > v9);
        ASSERT_TRUE(v3 < v9);
    }

    {
        Value v1 = "abc";
        Value v2 = " ";
        Value v3 = "";
        Value v4 = "0";
        Value v5(std::numeric_limits<double>::infinity());
        Value v6(-std::numeric_limits<double>::infinity());
        Value v7(std::numeric_limits<double>::quiet_NaN());

        // Infinity
        ASSERT_FALSE(v1 == v5);
        ASSERT_TRUE(v1 != v5);

        ASSERT_FALSE(v2 == v5);
        ASSERT_TRUE(v2 != v5);

        ASSERT_FALSE(v3 == v5);
        ASSERT_TRUE(v3 != v5);

        ASSERT_FALSE(v4 == v5);
        ASSERT_TRUE(v4 != v5);

        // -Infinity
        ASSERT_FALSE(v1 == v6);
        ASSERT_TRUE(v1 != v6);

        ASSERT_FALSE(v2 == v6);
        ASSERT_TRUE(v2 != v6);

        ASSERT_FALSE(v3 == v6);
        ASSERT_TRUE(v3 != v6);

        ASSERT_FALSE(v4 == v6);
        ASSERT_TRUE(v4 != v6);

        // NaN
        ASSERT_FALSE(v1 == v7);
        ASSERT_TRUE(v1 != v7);

        ASSERT_FALSE(v2 == v7);
        ASSERT_TRUE(v2 != v7);

        ASSERT_FALSE(v3 == v7);
        ASSERT_TRUE(v3 != v7);

        ASSERT_FALSE(v4 == v7);
        ASSERT_TRUE(v4 != v7);
    }
}

TEST(ValueTest, AssignStringPtr)
{
    {
        Value v;
        StringPtr *str = string_pool_new();
        string_assign_cstring(str, "test");
        value_assign_stringPtr(&v.data(), str);
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), ValueType::String);
    }

    {
        Value v;
        StringPtr *str = string_pool_new();
        string_assign_cstring(str, "Infinity");
        value_assign_stringPtr(&v.data(), str);
        ASSERT_EQ(v.toString(), "Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
    }

    {
        Value v;
        StringPtr *str = string_pool_new();
        string_assign_cstring(str, "-Infinity");
        value_assign_stringPtr(&v.data(), str);
        ASSERT_EQ(v.toString(), "-Infinity");
        ASSERT_EQ(v.type(), ValueType::String);
    }

    {
        Value v;
        StringPtr *str = string_pool_new();
        string_assign_cstring(str, "NaN");
        value_assign_stringPtr(&v.data(), str);
        ASSERT_EQ(v.toString(), "NaN");
        ASSERT_EQ(v.type(), ValueType::String);
    }
}

TEST(ValueTest, DoubleIsInt)
{
    ASSERT_TRUE(value_doubleIsInt(0.0));
    ASSERT_TRUE(value_doubleIsInt(15.0));
    ASSERT_TRUE(value_doubleIsInt(-468.0));
    ASSERT_FALSE(value_doubleIsInt(0.1));
    ASSERT_FALSE(value_doubleIsInt(1.2));
    ASSERT_FALSE(value_doubleIsInt(-12.5852));
}

TEST(ValueTest, DoubleToStringPtr)
{
    std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
    std::setlocale(LC_NUMERIC, "sk_SK.UTF-8");

    // TODO: Use a custom comparison function
    StringPtr *ret;
    ret = value_doubleToStringPtr(0.0);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "0");

    ret = value_doubleToStringPtr(-0.0);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "0");

    ret = value_doubleToStringPtr(2.0);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "2");

    ret = value_doubleToStringPtr(-2.0);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-2");

    ret = value_doubleToStringPtr(2.54);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "2.54");

    ret = value_doubleToStringPtr(-2.54);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-2.54");

    ret = value_doubleToStringPtr(59.8);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "59.8");

    ret = value_doubleToStringPtr(-59.8);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-59.8");

    ret = value_doubleToStringPtr(5.3);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "5.3");

    ret = value_doubleToStringPtr(-5.3);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-5.3");

    ret = value_doubleToStringPtr(2550.625021000115);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "2550.625021000115");

    ret = value_doubleToStringPtr(-2550.625021000115);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-2550.625021000115");

    ret = value_doubleToStringPtr(9.4324e+20);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "943240000000000000000");

    ret = value_doubleToStringPtr(-2.591e-2);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-0.02591");

    ret = value_doubleToStringPtr(9.4324e+21);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "9.4324e+21");

    ret = value_doubleToStringPtr(-2.591e-13);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-2.591e-13");

    ret = value_doubleToStringPtr(0.001);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "0.001");

    ret = value_doubleToStringPtr(-0.001);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-0.001");

    ret = value_doubleToStringPtr(0.000001);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "0.000001");

    ret = value_doubleToStringPtr(-0.000001);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-0.000001");

    ret = value_doubleToStringPtr(0.0000001);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "1e-7");

    ret = value_doubleToStringPtr(-0.0000001);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-1e-7");

    ret = value_doubleToStringPtr(std::numeric_limits<double>::infinity());
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "Infinity");

    ret = value_doubleToStringPtr(-std::numeric_limits<double>::infinity());
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "-Infinity");

    ret = value_doubleToStringPtr(std::numeric_limits<double>::quiet_NaN());
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "NaN");

    std::setlocale(LC_NUMERIC, oldLocale.c_str());
}

TEST(ValueTest, BoolToStringPtr)
{
    const StringPtr *ret;
    ret = value_boolToStringPtr(true);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "true");

    ret = value_boolToStringPtr(false);
    ASSERT_EQ(utf8::utf16to8(std::u16string(ret->data)), "false");
}

TEST(ValueTest, StringToDouble)
{
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("2147483647")), 2147483647.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-2147483647")), -2147483647.0);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("255.625")), 255.625);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-255.625")), -255.625);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0.15")), 0.15);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-0.15")), -0.15);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-0")), -0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0.0")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-0.0")), -0.0);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("+.15")), 0.15);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign(".15")), 0.15);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-.15")), -0.15);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("1.")), 1.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("+1.")), 1.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-1.")), -1.0);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0+5")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0-5")), 0.0);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("9432.4e-12")), 9.4324e-9);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-9432.4e-12")), -9.4324e-9);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("9432.4e+6")), 9.4324e+9);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-9432.4e+6")), -9.4324e+9);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("1 2 3")), 0.0);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("false")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("true")), 0.0);

    ASSERT_TRUE(value_stringToDouble(string_pool_new_assign("Infinity")) > 0 && std::isinf(value_stringToDouble(string_pool_new_assign("Infinity"))));
    ASSERT_TRUE(value_stringToDouble(string_pool_new_assign("-Infinity")) < 0 && std::isinf(value_stringToDouble(string_pool_new_assign("-Infinity"))));
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("NaN")), 0.0);

    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("something")), 0.0);

    // Hex
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0xafe")), 2814.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("   0xafe")), 2814.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0xafe   ")), 2814.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("   0xafe   ")), 2814.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0x0afe")), 2814.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0xBaCD")), 47821.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0XBaCD")), 47821.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0xAbG")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0xabf.d")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("+0xa")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-0xa")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0x+a")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0x-a")), 0.0);

    // Octal
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0o506")), 326.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("   0o506")), 326.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0o506   ")), 326.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("   0o506   ")), 326.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0o0506")), 326.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0O17206")), 7814.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0o5783")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0o573.2")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("+0o2")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-0o2")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0o+2")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0o-2")), 0.0);

    // Binary
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0b101101")), 45.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("   0b101101")), 45.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0b101101   ")), 45.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("   0b101101   ")), 45.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0b0101101")), 45.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0B1110100110")), 934.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0b100112001")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0b10011001.1")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("+0b1")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("-0b1")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0b+1")), 0.0);
    ASSERT_EQ(value_stringToDouble(string_pool_new_assign("0b-1")), 0.0);
}

TEST(ValueTest, StringToBool)
{
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("2147483647")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("-2147483647")));

    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("255.625")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("-255.625")));

    ASSERT_FALSE(value_stringToBool(string_pool_new_assign("0")));

    ASSERT_FALSE(value_stringToBool(string_pool_new_assign("false")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("true")));
    ASSERT_FALSE(value_stringToBool(string_pool_new_assign("FaLsE")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("TrUe")));

    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("Infinity")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("-Infinity")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("NaN")));

    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("something")));
    ASSERT_FALSE(value_stringToBool(string_pool_new_assign("")));

    // Hex
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0xafe")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("   0xafe")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0xafe   ")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("   0xafe   ")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0x0afe")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0xBaCD")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0x0")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0XBaCD")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0xAbG")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0xabf.d")));

    // Octal
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0o506")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("   0o506")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0o506   ")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("   0o506   ")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0o0506")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0O17206")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0o5783")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0o573.2")));

    // Binary
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0b101101")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("   0b101101")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0b101101   ")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("   0b101101   ")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0b0101101")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0B1110100110")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0b100112001")));
    ASSERT_TRUE(value_stringToBool(string_pool_new_assign("0b10011001.1")));
}

TEST(ValueTest, RedComponent)
{
    Rgb color = rgba(255, 100, 50, 255);
    ASSERT_EQ(red(color), 255);
}

TEST(ValueTest, GreenComponent)
{
    Rgb color = rgba(255, 100, 50, 255);
    ASSERT_EQ(green(color), 100);
}

TEST(ValueTest, BlueComponent)
{
    Rgb color = rgba(255, 100, 50, 255);
    ASSERT_EQ(blue(color), 50);
}

TEST(ValueTest, AlphaComponent)
{
    Rgb color = rgba(255, 100, 50, 128);
    ASSERT_EQ(alpha(color), 128);
}

TEST(ValueTest, RGB)
{
    Rgb color = rgb(255, 100, 50);
    ASSERT_EQ(color, 0xFFFF6432);
}

TEST(ValueTest, RGBA)
{
    Rgb color = rgba(255, 100, 50, 128);
    ASSERT_EQ(color, 0x80FF6432);
}
