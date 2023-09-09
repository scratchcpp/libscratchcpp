#include <scratchcpp/value.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(ValueTest, DefaultConstructor)
{
    Value v;
    ASSERT_EQ(v.toInt(), 0);
    ASSERT_EQ(v.type(), Value::Type::Integer);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, FloatConstructor)
{
    Value v(3.14f);
    ASSERT_EQ(v.toDouble(), 3.14f);
    ASSERT_EQ(v.type(), Value::Type::Double);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, DoubleConstructor)
{
    Value v(static_cast<double>(3.14));
    ASSERT_EQ(v.toDouble(), 3.14);
    ASSERT_EQ(v.type(), Value::Type::Double);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, IntConstructor)
{
    Value v(static_cast<int>(55));
    ASSERT_EQ(v.toInt(), 55);
    ASSERT_EQ(v.type(), Value::Type::Integer);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, SizeTConstructor)
{
    Value v(static_cast<size_t>(100));
    ASSERT_EQ(v.toLong(), 100);
    ASSERT_EQ(v.type(), Value::Type::Integer);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, LongConstructor)
{
    Value v(999999999999999999L);
    ASSERT_EQ(v.toLong(), 999999999999999999L);
    ASSERT_EQ(v.type(), Value::Type::Integer);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, BoolConstructor)
{
    {
        Value v(true);
        ASSERT_EQ(v.toBool(), true);
        ASSERT_EQ(v.type(), Value::Type::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(false);
        ASSERT_EQ(v.toBool(), false);
        ASSERT_EQ(v.type(), Value::Type::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, StdStringConstructor)
{
    {
        Value v(std::string("test"));
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), Value::Type::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("532"));
        ASSERT_EQ(v.toString(), "532");
        ASSERT_EQ(v.type(), Value::Type::Integer);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_TRUE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(std::string("1 2 3"));
        ASSERT_EQ(v.toString(), "1 2 3");
        ASSERT_EQ(v.type(), Value::Type::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v(std::string("Infinity"));
        ASSERT_EQ(v.toString(), "Infinity");
        ASSERT_EQ(v.type(), Value::Type::Infinity);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(std::string("-Infinity"));
        ASSERT_EQ(v.toString(), "-Infinity");
        ASSERT_EQ(v.type(), Value::Type::NegativeInfinity);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v(std::string("NaN"));
        ASSERT_EQ(v.toString(), "NaN");
        ASSERT_EQ(v.type(), Value::Type::NaN);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, CStringConstructor)
{
    {
        Value v("test");
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), Value::Type::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v("Infinity");
        ASSERT_EQ(v.toString(), "Infinity");
        ASSERT_EQ(v.type(), Value::Type::Infinity);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v("-Infinity");
        ASSERT_EQ(v.toString(), "-Infinity");
        ASSERT_EQ(v.type(), Value::Type::NegativeInfinity);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v("NaN");
        ASSERT_EQ(v.toString(), "NaN");
        ASSERT_EQ(v.type(), Value::Type::NaN);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, InfinityConstructor)
{
    Value v(Value::SpecialValue::Infinity);
    ASSERT_EQ(v.type(), Value::Type::Infinity);
    ASSERT_TRUE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_FALSE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, NegativeInfinityConstructor)
{
    Value v(Value::SpecialValue::NegativeInfinity);
    ASSERT_EQ(v.type(), Value::Type::NegativeInfinity);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_TRUE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_FALSE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, NaNConstructor)
{
    Value v(Value::SpecialValue::NaN);
    ASSERT_EQ(v.type(), Value::Type::NaN);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_TRUE(v.isNaN());
    ASSERT_FALSE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
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
        ASSERT_FALSE(v2.isBool());
        ASSERT_TRUE(v2.isString());
    }

    {
        Value v1(Value::SpecialValue::NaN);
        Value v2(v1);
        ASSERT_EQ(v2.toDouble(), 0);
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_TRUE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(Value::SpecialValue::Infinity);
        Value v2(v1);
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_TRUE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(Value::SpecialValue::NegativeInfinity);
        Value v2(v1);
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_TRUE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }
}

TEST(ValueTest, FloatAssignment)
{
    Value v;
    v = 3.14f;
    ASSERT_EQ(v.toDouble(), 3.14f);
    ASSERT_EQ(v.type(), Value::Type::Double);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, DoubleAssignment)
{
    Value v;
    v = static_cast<double>(3.14);
    ASSERT_EQ(v.toDouble(), 3.14);
    ASSERT_EQ(v.type(), Value::Type::Double);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, IntAssignment)
{
    Value v;
    v = static_cast<int>(55);
    ASSERT_EQ(v.toInt(), 55);
    ASSERT_EQ(v.type(), Value::Type::Integer);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, LongAssignment)
{
    Value v;
    v = 999999999999999999L;
    ASSERT_EQ(v.toLong(), 999999999999999999L);
    ASSERT_EQ(v.type(), Value::Type::Integer);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_TRUE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, BoolAssignment)
{
    {
        Value v;
        v = true;
        ASSERT_EQ(v.toBool(), true);
        ASSERT_EQ(v.type(), Value::Type::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_TRUE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v;
        v = false;
        ASSERT_EQ(v.toBool(), false);
        ASSERT_EQ(v.type(), Value::Type::Bool);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
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
        ASSERT_EQ(v.type(), Value::Type::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = std::string("Infinity");
        ASSERT_EQ(v.toString(), "Infinity");
        ASSERT_EQ(v.type(), Value::Type::Infinity);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v;
        v = std::string("-Infinity");
        ASSERT_EQ(v.toString(), "-Infinity");
        ASSERT_EQ(v.type(), Value::Type::NegativeInfinity);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v;
        v = std::string("NaN");
        ASSERT_EQ(v.toString(), "NaN");
        ASSERT_EQ(v.type(), Value::Type::NaN);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, CStringAssignment)
{
    {
        Value v;
        v = "test";
        ASSERT_EQ(v.toString(), "test");
        ASSERT_EQ(v.type(), Value::Type::String);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_TRUE(v.isString());
    }

    {
        Value v;
        v = "Infinity";
        ASSERT_EQ(v.toString(), "Infinity");
        ASSERT_EQ(v.type(), Value::Type::Infinity);
        ASSERT_TRUE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v;
        v = "-Infinity";
        ASSERT_EQ(v.toString(), "-Infinity");
        ASSERT_EQ(v.type(), Value::Type::NegativeInfinity);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_TRUE(v.isNegativeInfinity());
        ASSERT_FALSE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }

    {
        Value v;
        v = "NaN";
        ASSERT_EQ(v.toString(), "NaN");
        ASSERT_EQ(v.type(), Value::Type::NaN);
        ASSERT_FALSE(v.isInfinity());
        ASSERT_FALSE(v.isNegativeInfinity());
        ASSERT_TRUE(v.isNaN());
        ASSERT_FALSE(v.isNumber());
        ASSERT_FALSE(v.isBool());
        ASSERT_FALSE(v.isString());
    }
}

TEST(ValueTest, InfinityAssignment)
{
    Value v;
    v = Value::SpecialValue::Infinity;
    ASSERT_EQ(v.type(), Value::Type::Infinity);
    ASSERT_TRUE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_FALSE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, NegativeInfinityAssignment)
{
    Value v;
    v = Value::SpecialValue::NegativeInfinity;
    ASSERT_EQ(v.type(), Value::Type::NegativeInfinity);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_TRUE(v.isNegativeInfinity());
    ASSERT_FALSE(v.isNaN());
    ASSERT_FALSE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
}

TEST(ValueTest, NaNAssignment)
{
    Value v;
    v = Value::SpecialValue::NaN;
    ASSERT_EQ(v.type(), Value::Type::NaN);
    ASSERT_FALSE(v.isInfinity());
    ASSERT_FALSE(v.isNegativeInfinity());
    ASSERT_TRUE(v.isNaN());
    ASSERT_FALSE(v.isNumber());
    ASSERT_FALSE(v.isBool());
    ASSERT_FALSE(v.isString());
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
        ASSERT_FALSE(v2.isBool());
        ASSERT_TRUE(v2.isString());
    }

    {
        Value v1(Value::SpecialValue::NaN);
        Value v2;
        v2 = v1;
        ASSERT_EQ(v2.toDouble(), 0);
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_TRUE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(Value::SpecialValue::Infinity);
        Value v2;
        v2 = v1;
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_TRUE(v2.isInfinity());
        ASSERT_FALSE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
    }

    {
        Value v1(Value::SpecialValue::NegativeInfinity);
        Value v2;
        v2 = v1;
        ASSERT_EQ(v1.type(), v2.type());
        ASSERT_FALSE(v2.isInfinity());
        ASSERT_TRUE(v2.isNegativeInfinity());
        ASSERT_FALSE(v2.isNaN());
        ASSERT_FALSE(v2.isNumber());
        ASSERT_FALSE(v2.isBool());
        ASSERT_FALSE(v2.isString());
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
    v = 2.54f;
    ASSERT_EQ(v.toInt(), 2);

    v = -2.54;
    ASSERT_EQ(v.toInt(), -2);
    v = -2.54f;
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
    ASSERT_EQ(v.toInt(), std::numeric_limits<int>::infinity());
    v = "-Infinity";
    ASSERT_EQ(v.toInt(), -std::numeric_limits<int>::infinity());
    v = "NaN";
    ASSERT_EQ(v.toInt(), 0);

    v = "something";
    ASSERT_EQ(v.toInt(), 0);
}

TEST(ValueTest, ToLong)
{
    Value v = 999999999999999999L;
    ASSERT_EQ(v.toLong(), 999999999999999999L);
    v = -999999999999999999L;
    ASSERT_EQ(v.toLong(), -999999999999999999L);

    v = 2.54;
    ASSERT_EQ(v.toLong(), 2);
    v = 2.54f;
    ASSERT_EQ(v.toLong(), 2);

    v = -2.54;
    ASSERT_EQ(v.toLong(), -2);
    v = -2.54f;
    ASSERT_EQ(v.toLong(), -2);

    v = false;
    ASSERT_EQ(v.toLong(), 0);
    v = true;
    ASSERT_EQ(v.toLong(), 1);

    v = "999999999999999999";
    ASSERT_EQ(v.toLong(), 999999999999999999L);
    v = "-999999999999999999";
    ASSERT_EQ(v.toLong(), -999999999999999999L);

    v = "255.625";
    ASSERT_EQ(v.toLong(), 255);
    v = "-255.625";
    ASSERT_EQ(v.toLong(), -255);

    v = "false";
    ASSERT_EQ(v.toLong(), 0);
    v = "true";
    ASSERT_EQ(v.toLong(), 0); // booleans represented as string shouldn't convert

    v = "Infinity";
    ASSERT_EQ(v.toLong(), std::numeric_limits<long>::infinity());
    v = "-Infinity";
    ASSERT_EQ(v.toLong(), -std::numeric_limits<long>::infinity());
    v = "NaN";
    ASSERT_EQ(v.toLong(), 0);

    v = "something";
    ASSERT_EQ(v.toLong(), 0);
}

TEST(ValueTest, ToDouble)
{
    Value v = 2147483647;
    ASSERT_EQ(v.toDouble(), 2147483647.0);
    v = -2147483647;
    ASSERT_EQ(v.toDouble(), -2147483647.0);

    v = 2.54;
    ASSERT_EQ(v.toDouble(), 2.54);
    v = 2.54f;
    ASSERT_EQ(v.toDouble(), 2.54f);

    v = -2.54;
    ASSERT_EQ(v.toDouble(), -2.54);
    v = -2.54f;
    ASSERT_EQ(v.toDouble(), -2.54f);

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

    v = "false";
    ASSERT_EQ(v.toDouble(), 0.0);
    v = "true";
    ASSERT_EQ(v.toDouble(), 0.0); // booleans represented as string shouldn't convert

    v = "Infinity";
    ASSERT_EQ(v.toDouble(), std::numeric_limits<double>::infinity());
    v = "-Infinity";
    ASSERT_EQ(v.toDouble(), -std::numeric_limits<double>::infinity());
    v = "NaN";
    ASSERT_EQ(v.toDouble(), 0.0);

    v = "something";
    ASSERT_EQ(v.toDouble(), 0.0);
}

TEST(ValueTest, ToBool)
{
    Value v = 2147483647;
    ASSERT_EQ(v.toBool(), false);
    v = -2147483647;
    ASSERT_EQ(v.toBool(), false);

    v = 2.54;
    ASSERT_EQ(v.toBool(), false);
    v = 2.54f;
    ASSERT_EQ(v.toBool(), false);

    v = -2.54;
    ASSERT_EQ(v.toBool(), false);
    v = -2.54f;
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
    ASSERT_EQ(v.toBool(), false);

    v = "2147483647";
    ASSERT_EQ(v.toBool(), false);
    v = "-2147483647";
    ASSERT_EQ(v.toBool(), false);

    v = "255.625";
    ASSERT_EQ(v.toBool(), false);
    v = "-255.625";
    ASSERT_EQ(v.toBool(), false);

    v = "false";
    ASSERT_EQ(v.toBool(), false);
    v = "true";
    ASSERT_EQ(v.toBool(), true);

    v = "Infinity";
    ASSERT_EQ(v.toBool(), false);
    v = "-Infinity";
    ASSERT_EQ(v.toBool(), false);
    v = "NaN";
    ASSERT_EQ(v.toBool(), false);

    v = "something";
    ASSERT_EQ(v.toBool(), false);
}

TEST(ValueTest, ToString)
{
    Value v = 2147483647;
    ASSERT_EQ(v.toString(), "2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = -2147483647;
    ASSERT_EQ(v.toString(), "-2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = 999999999999999999L;
    ASSERT_EQ(v.toString(), "999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = -999999999999999999L;
    ASSERT_EQ(v.toString(), "-999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = 2.54;
    ASSERT_EQ(v.toString(), "2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = 2.54f;
    ASSERT_EQ(v.toString(), "2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = -2.54;
    ASSERT_EQ(v.toString(), "-2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = -2.54f;
    ASSERT_EQ(v.toString(), "-2.54");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = false;
    ASSERT_EQ(v.toString(), "false");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = true;
    ASSERT_EQ(v.toString(), "true");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = "2147483647";
    ASSERT_EQ(v.toString(), "2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = "-2147483647";
    ASSERT_EQ(v.toString(), "-2147483647");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = "999999999999999999";
    ASSERT_EQ(v.toString(), "999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = "-999999999999999999";
    ASSERT_EQ(v.toString(), "-999999999999999999");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = "255.625";
    ASSERT_EQ(v.toString(), "255.625");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = "-255.625";
    ASSERT_EQ(v.toString(), "-255.625");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = "false";
    ASSERT_EQ(v.toString(), "false");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = "true";
    ASSERT_EQ(v.toString(), "true");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = "Infinity";
    ASSERT_TRUE(v.isInfinity());
    ASSERT_EQ(v.toString(), "Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = "-Infinity";
    ASSERT_TRUE(v.isNegativeInfinity());
    ASSERT_EQ(v.toString(), "-Infinity");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
    v = "NaN";
    ASSERT_TRUE(v.isNaN());
    ASSERT_EQ(v.toString(), "NaN");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());

    v = "something";
    ASSERT_EQ(v.toString(), "something");
    ASSERT_EQ(utf8::utf16to8(v.toUtf16()), v.toString());
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

    v = 999999999999999999L;
    v.subtract(1);
    ASSERT_EQ(v.toLong(), 999999999999999998L);

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

    v = 999999999999999999L;
    v.multiply(2);
    ASSERT_EQ(v.toLong(), 1999999999999999998L);

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

TEST(ValueTest, EqualityOperators)
{
    {
        Value v1 = 10;
        Value v2 = 10;
        Value v3 = 8;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = -4.25;
        Value v2 = -4.25;
        Value v3 = 5.312;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = true;
        Value v2 = true;
        Value v3 = false;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
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

        ASSERT_FALSE(v2 == v4);
        ASSERT_TRUE(v2 != v4);

        ASSERT_FALSE(v3 == v4);
        ASSERT_TRUE(v3 != v4);
    }

    {
        Value v1(Value::SpecialValue::Infinity);
        Value v2(Value::SpecialValue::Infinity);
        Value v3(Value::SpecialValue::NegativeInfinity);
        Value v4(Value::SpecialValue::NegativeInfinity);
        Value v5(Value::SpecialValue::NaN);
        Value v6(Value::SpecialValue::NaN);

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_TRUE(v3 == v4);
        ASSERT_FALSE(v3 != v4);

        // TODO: Enable this after #196 is fixed
        /*ASSERT_TRUE(v5 == v6);
        ASSERT_FALSE(v5 != v6);*/

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

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

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = 1;
        Value v2 = true;
        Value v3 = false;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = 5.25;
        Value v2 = "5.25";
        Value v3 = "5.26";

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = 1;
        Value v2 = true;
        Value v3 = false;

        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 != v2);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);
    }

    {
        Value v1 = 5;
        Value v2 = 0;
        Value v3(Value::SpecialValue::Infinity);
        Value v4(Value::SpecialValue::NegativeInfinity);
        Value v5(Value::SpecialValue::NaN);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);

        ASSERT_FALSE(v1 == v5);
        ASSERT_TRUE(v1 != v5);

        // TODO: Enable this after #198 is fixed
        /*ASSERT_FALSE(v2 == v5);
        ASSERT_TRUE(v2 != v5);*/
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

        ASSERT_TRUE(v2 == v4);
        ASSERT_FALSE(v2 != v4);

        ASSERT_FALSE(v1 == v2);
        ASSERT_TRUE(v1 != v2);

        ASSERT_FALSE(v2 == v3);
        ASSERT_TRUE(v2 != v3);

        ASSERT_TRUE(v1 == v5);
        ASSERT_FALSE(v1 != v5);

        ASSERT_TRUE(v2 == v6);
        ASSERT_FALSE(v2 != v6);
    }

    {
        Value v1 = true;
        Value v2 = false;
        Value v3(Value::SpecialValue::Infinity);
        Value v4(Value::SpecialValue::NegativeInfinity);
        Value v5(Value::SpecialValue::NaN);

        ASSERT_FALSE(v1 == v3);
        ASSERT_TRUE(v1 != v3);

        ASSERT_FALSE(v1 == v4);
        ASSERT_TRUE(v1 != v4);

        ASSERT_FALSE(v1 == v5);
        ASSERT_TRUE(v1 != v5);

        ASSERT_FALSE(v2 == v5);
        ASSERT_TRUE(v2 != v5);
    }

    {
        Value v1 = "Infinity";
        Value v2 = "infinity";
        Value v3 = "-Infinity";
        Value v4 = "-infinity";
        Value v5 = "NaN";
        Value v6 = "nan";
        Value v7(Value::SpecialValue::Infinity);
        Value v8(Value::SpecialValue::NegativeInfinity);
        Value v9(Value::SpecialValue::NaN);

        // Infinity
        ASSERT_TRUE(v1 == v7);
        ASSERT_FALSE(v1 != v7);

        ASSERT_FALSE(v1 == v8);
        ASSERT_TRUE(v1 != v8);

        ASSERT_FALSE(v1 == v9);
        ASSERT_TRUE(v1 != v9);

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

        // TODO: Enable this after #197 is fixed
        /*ASSERT_TRUE(v5 == v9);
        ASSERT_FALSE(v5 != v9);*/

        // nan
        ASSERT_FALSE(v6 == v7);
        ASSERT_TRUE(v6 != v7);

        ASSERT_FALSE(v6 == v8);
        ASSERT_TRUE(v6 != v8);

        ASSERT_TRUE(v6 == v9);
        ASSERT_FALSE(v6 != v9);
    }

    {
        Value v1 = "abc";
        Value v2 = " ";
        Value v3 = "";
        Value v4 = "0";
        Value v5(Value::SpecialValue::Infinity);
        Value v6(Value::SpecialValue::NegativeInfinity);
        Value v7(Value::SpecialValue::NaN);

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

        // TODO: Enable this after #198 is fixed
        /*ASSERT_FALSE(v4 == v7);
        ASSERT_TRUE(v4 != v7);*/
    }
}
