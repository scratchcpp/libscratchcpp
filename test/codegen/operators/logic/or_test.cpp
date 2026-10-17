#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class OrTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(OrTest, NumberWithNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 10, 8);
}

TEST_F(OrTest, NumberWithNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 10, 8);
}

TEST_F(OrTest, SameNegativeDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, -4.25, -4.25);
}

TEST_F(OrTest, SameNegativeDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, -4.25, -4.25);
}

TEST_F(OrTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, -4.25, 5.312);
}

TEST_F(OrTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, -4.25, 5.312);
}

TEST_F(OrTest, TrueAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, true);
}

TEST_F(OrTest, TrueAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, true);
}

TEST_F(OrTest, TrueAndFalse)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, false);
}

TEST_F(OrTest, TrueAndFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, false);
}

TEST_F(OrTest, FalseAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, true);
}

TEST_F(OrTest, FalseAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, true);
}

TEST_F(OrTest, NumberAndTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 1, true);
}

TEST_F(OrTest, NumberAndTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 1, true);
}

TEST_F(OrTest, NumberAndFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 1, false);
}

TEST_F(OrTest, NumberAndFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 1, false);
}

TEST_F(OrTest, StringAndString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "abc", "def");
}

TEST_F(OrTest, StringAndString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "abc", "def");
}

TEST_F(OrTest, TrueStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", "true");
}

TEST_F(OrTest, TrueStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", "true");
}

TEST_F(OrTest, TrueStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", "false");
}

TEST_F(OrTest, TrueStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", "false");
}

TEST_F(OrTest, FalseStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", "true");
}

TEST_F(OrTest, FalseStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", "true");
}

TEST_F(OrTest, FalseStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", "false");
}

TEST_F(OrTest, FalseStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", "false");
}

TEST_F(OrTest, NumberAndExactString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 5.25, "5.25");
}

TEST_F(OrTest, NumberAndExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 5.25, "5.25");
}

TEST_F(OrTest, ExactStringAndNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "5.25", 5.25);
}

TEST_F(OrTest, ExactStringAndNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "5.25", 5.25);
}

TEST_F(OrTest, ZeroAndOneString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 0, "1");
}

TEST_F(OrTest, ZeroAndOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 0, "1");
}

TEST_F(OrTest, OneStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "1", 0);
}

TEST_F(OrTest, OneStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "1", 0);
}

TEST_F(OrTest, ZeroAndTestString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 0, "test");
}

TEST_F(OrTest, ZeroAndTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 0, "test");
}

TEST_F(OrTest, TestStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "test", 0);
}

TEST_F(OrTest, TestStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "test", 0);
}

TEST_F(OrTest, InfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", "Infinity");
}

TEST_F(OrTest, InfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", "Infinity");
}

TEST_F(OrTest, NegativeInfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", "-Infinity");
}

TEST_F(OrTest, NegativeInfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", "-Infinity");
}

TEST_F(OrTest, NaNAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", "NaN");
}

TEST_F(OrTest, NaNAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", "NaN");
}

TEST_F(OrTest, InfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", "-Infinity");
}

TEST_F(OrTest, InfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", "-Infinity");
}

TEST_F(OrTest, NegativeInfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", "Infinity");
}

TEST_F(OrTest, NegativeInfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", "Infinity");
}

TEST_F(OrTest, InfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", "NaN");
}

TEST_F(OrTest, InfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", "NaN");
}

TEST_F(OrTest, NaNAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", "Infinity");
}

TEST_F(OrTest, NaNAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", "Infinity");
}

TEST_F(OrTest, NegativeInfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", "NaN");
}

TEST_F(OrTest, NegativeInfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", "NaN");
}

TEST_F(OrTest, NaNAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", "-Infinity");
}

TEST_F(OrTest, NaNAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", "-Infinity");
}

TEST_F(OrTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "true");
}

TEST_F(OrTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "true");
}

TEST_F(OrTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", true);
}

TEST_F(OrTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", true);
}

TEST_F(OrTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, "false");
}

TEST_F(OrTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, "false");
}

TEST_F(OrTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", false);
}

TEST_F(OrTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", false);
}

TEST_F(OrTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, "true");
}

TEST_F(OrTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, "true");
}

TEST_F(OrTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", false);
}

TEST_F(OrTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", false);
}

TEST_F(OrTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "false");
}

TEST_F(OrTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "false");
}

TEST_F(OrTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", true);
}

TEST_F(OrTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", true);
}

TEST_F(OrTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "TRUE");
}

TEST_F(OrTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "TRUE");
}

TEST_F(OrTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "TRUE", true);
}

TEST_F(OrTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "TRUE", true);
}

TEST_F(OrTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, "FALSE");
}

TEST_F(OrTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, "FALSE");
}

TEST_F(OrTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "FALSE", false);
}

TEST_F(OrTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "FALSE", false);
}

TEST_F(OrTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "00001");
}

TEST_F(OrTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "00001");
}

TEST_F(OrTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "00001", true);
}

TEST_F(OrTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "00001", true);
}

TEST_F(OrTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "00000");
}

TEST_F(OrTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "00000");
}

TEST_F(OrTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "00000", true);
}

TEST_F(OrTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "00000", true);
}

TEST_F(OrTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, "00000");
}

TEST_F(OrTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, "00000");
}

TEST_F(OrTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "00000", false);
}

TEST_F(OrTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "00000", false);
}

TEST_F(OrTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", 1);
}

TEST_F(OrTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", 1);
}

TEST_F(OrTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 1, "true");
}

TEST_F(OrTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 1, "true");
}

TEST_F(OrTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", 0);
}

TEST_F(OrTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", 0);
}

TEST_F(OrTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 0, "true");
}

TEST_F(OrTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 0, "true");
}

TEST_F(OrTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", 0);
}

TEST_F(OrTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", 0);
}

TEST_F(OrTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 0, "false");
}

TEST_F(OrTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 0, "false");
}

TEST_F(OrTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", 1);
}

TEST_F(OrTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", 1);
}

TEST_F(OrTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, 1, "false");
}

TEST_F(OrTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, 1, "false");
}

TEST_F(OrTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", "TRUE");
}

TEST_F(OrTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", "TRUE");
}

TEST_F(OrTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "true", "FALSE");
}

TEST_F(OrTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "true", "FALSE");
}

TEST_F(OrTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", "FALSE");
}

TEST_F(OrTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", "FALSE");
}

TEST_F(OrTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "false", "TRUE");
}

TEST_F(OrTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "false", "TRUE");
}

TEST_F(OrTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "Infinity");
}

TEST_F(OrTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "Infinity");
}

TEST_F(OrTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", true);
}

TEST_F(OrTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", true);
}

TEST_F(OrTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "-Infinity");
}

TEST_F(OrTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "-Infinity");
}

TEST_F(OrTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", true);
}

TEST_F(OrTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", true);
}

TEST_F(OrTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, true, "NaN");
}

TEST_F(OrTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, true, "NaN");
}

TEST_F(OrTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", true);
}

TEST_F(OrTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", true);
}

TEST_F(OrTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, "Infinity");
}

TEST_F(OrTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, "Infinity");
}

TEST_F(OrTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", false);
}

TEST_F(OrTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", false);
}

TEST_F(OrTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, "-Infinity");
}

TEST_F(OrTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, "-Infinity");
}

TEST_F(OrTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", false);
}

TEST_F(OrTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", false);
}

TEST_F(OrTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, false, "NaN");
}

TEST_F(OrTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, false, "NaN");
}

TEST_F(OrTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", false);
}

TEST_F(OrTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", false);
}

TEST_F(OrTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", "Infinity");
}

TEST_F(OrTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", "Infinity");
}

TEST_F(OrTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", "-Infinity");
}

TEST_F(OrTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", "-Infinity");
}

TEST_F(OrTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "Infinity", "NaN");
}

TEST_F(OrTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "Infinity", "NaN");
}

TEST_F(OrTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "infinity", "Infinity");
}

TEST_F(OrTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "infinity", "Infinity");
}

TEST_F(OrTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "infinity", "-Infinity");
}

TEST_F(OrTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "infinity", "-Infinity");
}

TEST_F(OrTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "infinity", "NaN");
}

TEST_F(OrTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "infinity", "NaN");
}

TEST_F(OrTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", "Infinity");
}

TEST_F(OrTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", "Infinity");
}

TEST_F(OrTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", "-Infinity");
}

TEST_F(OrTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", "-Infinity");
}

TEST_F(OrTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-Infinity", "NaN");
}

TEST_F(OrTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-Infinity", "NaN");
}

TEST_F(OrTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-infinity", "Infinity");
}

TEST_F(OrTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-infinity", "Infinity");
}

TEST_F(OrTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-infinity", "-Infinity");
}

TEST_F(OrTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-infinity", "-Infinity");
}

TEST_F(OrTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "-infinity", "NaN");
}

TEST_F(OrTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "-infinity", "NaN");
}

TEST_F(OrTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", "Infinity");
}

TEST_F(OrTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", "Infinity");
}

TEST_F(OrTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", "-Infinity");
}

TEST_F(OrTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", "-Infinity");
}

TEST_F(OrTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "NaN", "NaN");
}

TEST_F(OrTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "NaN", "NaN");
}

TEST_F(OrTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "nan", "Infinity");
}

TEST_F(OrTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "nan", "Infinity");
}

TEST_F(OrTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "nan", "-Infinity");
}

TEST_F(OrTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "nan", "-Infinity");
}

TEST_F(OrTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, false, "nan", "NaN");
}

TEST_F(OrTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::Or, true, "nan", "NaN");
}
