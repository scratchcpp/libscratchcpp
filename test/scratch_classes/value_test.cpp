#include <scratchcpp/value.h>
#include <utf8.h>

#include "../common.h"

using namespace libscratchcpp;

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
    std::vector<char *> cStrings;
    Value v = 2147483647;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = -2147483647;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 512L;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "512");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = -512L;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-512");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 0.0;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "0");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = -0.0;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "0");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 2.0;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "2");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = -2.0;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-2");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 2.54;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = -2.54;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 2550.625021000115;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "2550.625021000115");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = -2550.625021000115;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-2550.625021000115");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 9.4324e+20;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "943240000000000000000");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = -2.591e-2;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-0.02591");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 9.4324e+21;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "9.4324e+21");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = -2.591e-13;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-2.591e-13");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = 0.001;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "0.001");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = -0.001;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-0.001");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = false;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "false");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = true;
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "true");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "2147483647";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "-2147483647";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "999999999999999999";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "-999999999999999999";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "255.625";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "255.625");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "-255.625";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-255.625");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "9432.4e-12";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "9432.4e-12");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "-9432.4e-12";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-9432.4e-12");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "9432.4e+6";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "9432.4e+6");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "-9432.4e+6";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-9432.4e+6");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "false";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "false");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "true";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "true");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "Infinity";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_TRUE(v.isInfinity());
    ASSERT_EQ(v.toString(), "Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "-Infinity";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_TRUE(v.isNegativeInfinity());
    ASSERT_EQ(v.toString(), "-Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = "NaN";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_TRUE(v.isNaN());
    ASSERT_EQ(v.toString(), "NaN");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = "something";
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "something");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    v = std::numeric_limits<double>::infinity();
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = -std::numeric_limits<double>::infinity();
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "-Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());
    v = std::numeric_limits<double>::quiet_NaN();
    cStrings.push_back(value_toCString(&v.data()));
    ASSERT_EQ(v.toString(), "NaN");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    ASSERT_EQ(std::string(cStrings.back()), v.toString());

    for (char *s : cStrings)
        free(s);
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

TEST(ValueTest, DoubleToCString)
{
    char *ret;
    ret = value_doubleToCString(0.0);
    ASSERT_EQ(strcmp(ret, "0"), 0);
    free(ret);

    ret = value_doubleToCString(-0.0);
    ASSERT_EQ(strcmp(ret, "0"), 0);
    free(ret);

    ret = value_doubleToCString(2.0);
    ASSERT_EQ(strcmp(ret, "2"), 0);
    free(ret);

    ret = value_doubleToCString(-2.0);
    ASSERT_EQ(strcmp(ret, "-2"), 0);
    free(ret);

    ret = value_doubleToCString(2.54);
    ASSERT_EQ(strcmp(ret, "2.54"), 0);
    free(ret);

    ret = value_doubleToCString(-2.54);
    ASSERT_EQ(strcmp(ret, "-2.54"), 0);
    free(ret);

    ret = value_doubleToCString(2550.625021000115);
    ASSERT_EQ(strcmp(ret, "2550.625021000115"), 0);
    free(ret);

    ret = value_doubleToCString(-2550.625021000115);
    ASSERT_EQ(strcmp(ret, "-2550.625021000115"), 0);
    free(ret);

    ret = value_doubleToCString(9.4324e+20);
    ASSERT_EQ(strcmp(ret, "943240000000000000000"), 0);
    free(ret);

    ret = value_doubleToCString(-2.591e-2);
    ASSERT_EQ(strcmp(ret, "-0.02591"), 0);
    free(ret);

    ret = value_doubleToCString(9.4324e+21);
    ASSERT_EQ(strcmp(ret, "9.4324e+21"), 0);
    free(ret);

    ret = value_doubleToCString(-2.591e-13);
    ASSERT_EQ(strcmp(ret, "-2.591e-13"), 0);
    free(ret);

    ret = value_doubleToCString(0.001);
    ASSERT_EQ(strcmp(ret, "0.001"), 0);
    free(ret);

    ret = value_doubleToCString(-0.001);
    ASSERT_EQ(strcmp(ret, "-0.001"), 0);
    free(ret);

    ret = value_doubleToCString(std::numeric_limits<double>::infinity());
    ASSERT_EQ(strcmp(ret, "Infinity"), 0);
    free(ret);

    ret = value_doubleToCString(-std::numeric_limits<double>::infinity());
    ASSERT_EQ(strcmp(ret, "-Infinity"), 0);
    free(ret);

    ret = value_doubleToCString(std::numeric_limits<double>::quiet_NaN());
    ASSERT_EQ(strcmp(ret, "NaN"), 0);
    free(ret);
}

TEST(ValueTest, BoolToCString)
{
    const char *ret;
    ret = value_boolToCString(true);
    ASSERT_EQ(strcmp(ret, "true"), 0);

    ret = value_boolToCString(false);
    ASSERT_EQ(strcmp(ret, "false"), 0);
}

TEST(ValueTest, StringToDouble)
{
    ASSERT_EQ(value_stringToDouble("2147483647"), 2147483647.0);
    ASSERT_EQ(value_stringToDouble("-2147483647"), -2147483647.0);

    ASSERT_EQ(value_stringToDouble("255.625"), 255.625);
    ASSERT_EQ(value_stringToDouble("-255.625"), -255.625);

    ASSERT_EQ(value_stringToDouble("0.15"), 0.15);
    ASSERT_EQ(value_stringToDouble("-0.15"), -0.15);

    ASSERT_EQ(value_stringToDouble("0"), 0.0);
    ASSERT_EQ(value_stringToDouble("-0"), -0.0);
    ASSERT_EQ(value_stringToDouble("0.0"), 0.0);
    ASSERT_EQ(value_stringToDouble("-0.0"), -0.0);

    ASSERT_EQ(value_stringToDouble("+.15"), 0.15);
    ASSERT_EQ(value_stringToDouble(".15"), 0.15);
    ASSERT_EQ(value_stringToDouble("-.15"), -0.15);

    ASSERT_EQ(value_stringToDouble("1."), 1.0);
    ASSERT_EQ(value_stringToDouble("+1."), 1.0);
    ASSERT_EQ(value_stringToDouble("-1."), -1.0);

    ASSERT_EQ(value_stringToDouble("0+5"), 0.0);
    ASSERT_EQ(value_stringToDouble("0-5"), 0.0);

    ASSERT_EQ(value_stringToDouble("9432.4e-12"), 9.4324e-9);
    ASSERT_EQ(value_stringToDouble("-9432.4e-12"), -9.4324e-9);

    ASSERT_EQ(value_stringToDouble("9432.4e+6"), 9.4324e+9);
    ASSERT_EQ(value_stringToDouble("-9432.4e+6"), -9.4324e+9);

    ASSERT_EQ(value_stringToDouble("1 2 3"), 0.0);

    ASSERT_EQ(value_stringToDouble("false"), 0.0);
    ASSERT_EQ(value_stringToDouble("true"), 0.0);

    ASSERT_TRUE(value_stringToDouble("Infinity") > 0 && std::isinf(value_stringToDouble("Infinity")));
    ASSERT_TRUE(value_stringToDouble("-Infinity") < 0 && std::isinf(value_stringToDouble("-Infinity")));
    ASSERT_EQ(value_stringToDouble("NaN"), 0.0);

    ASSERT_EQ(value_stringToDouble("something"), 0.0);

    // Hex
    ASSERT_EQ(value_stringToDouble("0xafe"), 2814.0);
    ASSERT_EQ(value_stringToDouble("   0xafe"), 2814.0);
    ASSERT_EQ(value_stringToDouble("0xafe   "), 2814.0);
    ASSERT_EQ(value_stringToDouble("   0xafe   "), 2814.0);
    ASSERT_EQ(value_stringToDouble("0x0afe"), 2814.0);
    ASSERT_EQ(value_stringToDouble("0xBaCD"), 47821.0);
    ASSERT_EQ(value_stringToDouble("0XBaCD"), 47821.0);
    ASSERT_EQ(value_stringToDouble("0xAbG"), 0.0);
    ASSERT_EQ(value_stringToDouble("0xabf.d"), 0.0);
    ASSERT_EQ(value_stringToDouble("+0xa"), 0.0);
    ASSERT_EQ(value_stringToDouble("-0xa"), 0.0);
    ASSERT_EQ(value_stringToDouble("0x+a"), 0.0);
    ASSERT_EQ(value_stringToDouble("0x-a"), 0.0);

    // Octal
    ASSERT_EQ(value_stringToDouble("0o506"), 326.0);
    ASSERT_EQ(value_stringToDouble("   0o506"), 326.0);
    ASSERT_EQ(value_stringToDouble("0o506   "), 326.0);
    ASSERT_EQ(value_stringToDouble("   0o506   "), 326.0);
    ASSERT_EQ(value_stringToDouble("0o0506"), 326.0);
    ASSERT_EQ(value_stringToDouble("0O17206"), 7814.0);
    ASSERT_EQ(value_stringToDouble("0o5783"), 0.0);
    ASSERT_EQ(value_stringToDouble("0o573.2"), 0.0);
    ASSERT_EQ(value_stringToDouble("+0o2"), 0.0);
    ASSERT_EQ(value_stringToDouble("-0o2"), 0.0);
    ASSERT_EQ(value_stringToDouble("0o+2"), 0.0);
    ASSERT_EQ(value_stringToDouble("0o-2"), 0.0);

    // Binary
    ASSERT_EQ(value_stringToDouble("0b101101"), 45.0);
    ASSERT_EQ(value_stringToDouble("   0b101101"), 45.0);
    ASSERT_EQ(value_stringToDouble("0b101101   "), 45.0);
    ASSERT_EQ(value_stringToDouble("   0b101101   "), 45.0);
    ASSERT_EQ(value_stringToDouble("0b0101101"), 45.0);
    ASSERT_EQ(value_stringToDouble("0B1110100110"), 934.0);
    ASSERT_EQ(value_stringToDouble("0b100112001"), 0.0);
    ASSERT_EQ(value_stringToDouble("0b10011001.1"), 0.0);
    ASSERT_EQ(value_stringToDouble("+0b1"), 0.0);
    ASSERT_EQ(value_stringToDouble("-0b1"), 0.0);
    ASSERT_EQ(value_stringToDouble("0b+1"), 0.0);
    ASSERT_EQ(value_stringToDouble("0b-1"), 0.0);
}

TEST(ValueTest, StringToBool)
{
    ASSERT_EQ(value_stringToBool("2147483647"), true);
    ASSERT_EQ(value_stringToBool("-2147483647"), true);

    ASSERT_EQ(value_stringToBool("255.625"), true);
    ASSERT_EQ(value_stringToBool("-255.625"), true);

    ASSERT_EQ(value_stringToBool("0"), false);

    ASSERT_EQ(value_stringToBool("false"), false);
    ASSERT_EQ(value_stringToBool("true"), true);
    ASSERT_EQ(value_stringToBool("FaLsE"), false);
    ASSERT_EQ(value_stringToBool("TrUe"), true);

    ASSERT_EQ(value_stringToBool("Infinity"), true);
    ASSERT_EQ(value_stringToBool("-Infinity"), true);
    ASSERT_EQ(value_stringToBool("NaN"), true);

    ASSERT_EQ(value_stringToBool("something"), true);
    ASSERT_EQ(value_stringToBool(""), false);

    // Hex
    ASSERT_TRUE(value_stringToBool("0xafe"));
    ASSERT_TRUE(value_stringToBool("   0xafe"));
    ASSERT_TRUE(value_stringToBool("0xafe   "));
    ASSERT_TRUE(value_stringToBool("   0xafe   "));
    ASSERT_TRUE(value_stringToBool("0x0afe"));
    ASSERT_TRUE(value_stringToBool("0xBaCD"));
    ASSERT_TRUE(value_stringToBool("0x0"));
    ASSERT_TRUE(value_stringToBool("0XBaCD"));
    ASSERT_TRUE(value_stringToBool("0xAbG"));
    ASSERT_TRUE(value_stringToBool("0xabf.d"));

    // Octal
    ASSERT_TRUE(value_stringToBool("0o506"));
    ASSERT_TRUE(value_stringToBool("   0o506"));
    ASSERT_TRUE(value_stringToBool("0o506   "));
    ASSERT_TRUE(value_stringToBool("   0o506   "));
    ASSERT_TRUE(value_stringToBool("0o0506"));
    ASSERT_TRUE(value_stringToBool("0O17206"));
    ASSERT_TRUE(value_stringToBool("0o5783"));
    ASSERT_TRUE(value_stringToBool("0o573.2"));

    // Binary
    ASSERT_TRUE(value_stringToBool("0b101101"));
    ASSERT_TRUE(value_stringToBool("   0b101101"));
    ASSERT_TRUE(value_stringToBool("0b101101   "));
    ASSERT_TRUE(value_stringToBool("   0b101101   "));
    ASSERT_TRUE(value_stringToBool("0b0101101"));
    ASSERT_TRUE(value_stringToBool("0B1110100110"));
    ASSERT_TRUE(value_stringToBool("0b100112001"));
    ASSERT_TRUE(value_stringToBool("0b10011001.1"));
}
