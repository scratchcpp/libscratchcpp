#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class AndTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(AndTest, NumberWithNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 10, 8);
}

TEST_F(AndTest, NumberWithNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 10, 8);
}

TEST_F(AndTest, SameNegativeDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, -4.25, -4.25);
}

TEST_F(AndTest, SameNegativeDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, -4.25, -4.25);
}

TEST_F(AndTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, -4.25, 5.312);
}

TEST_F(AndTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, -4.25, 5.312);
}

TEST_F(AndTest, TrueAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, true);
}

TEST_F(AndTest, TrueAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, true);
}

TEST_F(AndTest, TrueAndFalse)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, false);
}

TEST_F(AndTest, TrueAndFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, false);
}

TEST_F(AndTest, FalseAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, true);
}

TEST_F(AndTest, FalseAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, true);
}

TEST_F(AndTest, NumberAndTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 1, true);
}

TEST_F(AndTest, NumberAndTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 1, true);
}

TEST_F(AndTest, NumberAndFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 1, false);
}

TEST_F(AndTest, NumberAndFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 1, false);
}

TEST_F(AndTest, StringAndString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "abc", "def");
}

TEST_F(AndTest, StringAndString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "abc", "def");
}

TEST_F(AndTest, TrueStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", "true");
}

TEST_F(AndTest, TrueStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", "true");
}

TEST_F(AndTest, TrueStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", "false");
}

TEST_F(AndTest, TrueStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", "false");
}

TEST_F(AndTest, FalseStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", "true");
}

TEST_F(AndTest, FalseStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", "true");
}

TEST_F(AndTest, FalseStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", "false");
}

TEST_F(AndTest, FalseStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", "false");
}

TEST_F(AndTest, NumberAndExactString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 5.25, "5.25");
}

TEST_F(AndTest, NumberAndExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 5.25, "5.25");
}

TEST_F(AndTest, ExactStringAndNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "5.25", 5.25);
}

TEST_F(AndTest, ExactStringAndNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "5.25", 5.25);
}

TEST_F(AndTest, ZeroAndOneString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 0, "1");
}

TEST_F(AndTest, ZeroAndOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 0, "1");
}

TEST_F(AndTest, OneStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "1", 0);
}

TEST_F(AndTest, OneStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "1", 0);
}

TEST_F(AndTest, ZeroAndTestString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 0, "test");
}

TEST_F(AndTest, ZeroAndTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 0, "test");
}

TEST_F(AndTest, TestStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "test", 0);
}

TEST_F(AndTest, TestStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "test", 0);
}

TEST_F(AndTest, InfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", "Infinity");
}

TEST_F(AndTest, InfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", "Infinity");
}

TEST_F(AndTest, NegativeInfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", "-Infinity");
}

TEST_F(AndTest, NegativeInfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", "-Infinity");
}

TEST_F(AndTest, NaNAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", "NaN");
}

TEST_F(AndTest, NaNAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", "NaN");
}

TEST_F(AndTest, InfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", "-Infinity");
}

TEST_F(AndTest, InfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", "-Infinity");
}

TEST_F(AndTest, NegativeInfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", "Infinity");
}

TEST_F(AndTest, NegativeInfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", "Infinity");
}

TEST_F(AndTest, InfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", "NaN");
}

TEST_F(AndTest, InfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", "NaN");
}

TEST_F(AndTest, NaNAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", "Infinity");
}

TEST_F(AndTest, NaNAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", "Infinity");
}

TEST_F(AndTest, NegativeInfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", "NaN");
}

TEST_F(AndTest, NegativeInfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", "NaN");
}

TEST_F(AndTest, NaNAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", "-Infinity");
}

TEST_F(AndTest, NaNAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", "-Infinity");
}

TEST_F(AndTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "true");
}

TEST_F(AndTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "true");
}

TEST_F(AndTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", true);
}

TEST_F(AndTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", true);
}

TEST_F(AndTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, "false");
}

TEST_F(AndTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, "false");
}

TEST_F(AndTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", false);
}

TEST_F(AndTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", false);
}

TEST_F(AndTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, "true");
}

TEST_F(AndTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, "true");
}

TEST_F(AndTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", false);
}

TEST_F(AndTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", false);
}

TEST_F(AndTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "false");
}

TEST_F(AndTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "false");
}

TEST_F(AndTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", true);
}

TEST_F(AndTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", true);
}

TEST_F(AndTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "TRUE");
}

TEST_F(AndTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "TRUE");
}

TEST_F(AndTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "TRUE", true);
}

TEST_F(AndTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "TRUE", true);
}

TEST_F(AndTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, "FALSE");
}

TEST_F(AndTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, "FALSE");
}

TEST_F(AndTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "FALSE", false);
}

TEST_F(AndTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "FALSE", false);
}

TEST_F(AndTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "00001");
}

TEST_F(AndTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "00001");
}

TEST_F(AndTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "00001", true);
}

TEST_F(AndTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "00001", true);
}

TEST_F(AndTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "00000");
}

TEST_F(AndTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "00000");
}

TEST_F(AndTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "00000", true);
}

TEST_F(AndTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "00000", true);
}

TEST_F(AndTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, "00000");
}

TEST_F(AndTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, "00000");
}

TEST_F(AndTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "00000", false);
}

TEST_F(AndTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "00000", false);
}

TEST_F(AndTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", 1);
}

TEST_F(AndTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", 1);
}

TEST_F(AndTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 1, "true");
}

TEST_F(AndTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 1, "true");
}

TEST_F(AndTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", 0);
}

TEST_F(AndTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", 0);
}

TEST_F(AndTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 0, "true");
}

TEST_F(AndTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 0, "true");
}

TEST_F(AndTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", 0);
}

TEST_F(AndTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", 0);
}

TEST_F(AndTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 0, "false");
}

TEST_F(AndTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 0, "false");
}

TEST_F(AndTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", 1);
}

TEST_F(AndTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", 1);
}

TEST_F(AndTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, 1, "false");
}

TEST_F(AndTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, 1, "false");
}

TEST_F(AndTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", "TRUE");
}

TEST_F(AndTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", "TRUE");
}

TEST_F(AndTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "true", "FALSE");
}

TEST_F(AndTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "true", "FALSE");
}

TEST_F(AndTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", "FALSE");
}

TEST_F(AndTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", "FALSE");
}

TEST_F(AndTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "false", "TRUE");
}

TEST_F(AndTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "false", "TRUE");
}

TEST_F(AndTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "Infinity");
}

TEST_F(AndTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "Infinity");
}

TEST_F(AndTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", true);
}

TEST_F(AndTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", true);
}

TEST_F(AndTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "-Infinity");
}

TEST_F(AndTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "-Infinity");
}

TEST_F(AndTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", true);
}

TEST_F(AndTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", true);
}

TEST_F(AndTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, true, "NaN");
}

TEST_F(AndTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, true, "NaN");
}

TEST_F(AndTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", true);
}

TEST_F(AndTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", true);
}

TEST_F(AndTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, "Infinity");
}

TEST_F(AndTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, "Infinity");
}

TEST_F(AndTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", false);
}

TEST_F(AndTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", false);
}

TEST_F(AndTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, "-Infinity");
}

TEST_F(AndTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, "-Infinity");
}

TEST_F(AndTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", false);
}

TEST_F(AndTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", false);
}

TEST_F(AndTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, false, "NaN");
}

TEST_F(AndTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, false, "NaN");
}

TEST_F(AndTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", false);
}

TEST_F(AndTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", false);
}

TEST_F(AndTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", "Infinity");
}

TEST_F(AndTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", "Infinity");
}

TEST_F(AndTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", "-Infinity");
}

TEST_F(AndTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", "-Infinity");
}

TEST_F(AndTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "Infinity", "NaN");
}

TEST_F(AndTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "Infinity", "NaN");
}

TEST_F(AndTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "infinity", "Infinity");
}

TEST_F(AndTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "infinity", "Infinity");
}

TEST_F(AndTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "infinity", "-Infinity");
}

TEST_F(AndTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "infinity", "-Infinity");
}

TEST_F(AndTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "infinity", "NaN");
}

TEST_F(AndTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "infinity", "NaN");
}

TEST_F(AndTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", "Infinity");
}

TEST_F(AndTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", "Infinity");
}

TEST_F(AndTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", "-Infinity");
}

TEST_F(AndTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", "-Infinity");
}

TEST_F(AndTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-Infinity", "NaN");
}

TEST_F(AndTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-Infinity", "NaN");
}

TEST_F(AndTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-infinity", "Infinity");
}

TEST_F(AndTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-infinity", "Infinity");
}

TEST_F(AndTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-infinity", "-Infinity");
}

TEST_F(AndTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-infinity", "-Infinity");
}

TEST_F(AndTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "-infinity", "NaN");
}

TEST_F(AndTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "-infinity", "NaN");
}

TEST_F(AndTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", "Infinity");
}

TEST_F(AndTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", "Infinity");
}

TEST_F(AndTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", "-Infinity");
}

TEST_F(AndTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", "-Infinity");
}

TEST_F(AndTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "NaN", "NaN");
}

TEST_F(AndTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "NaN", "NaN");
}

TEST_F(AndTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "nan", "Infinity");
}

TEST_F(AndTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "nan", "Infinity");
}

TEST_F(AndTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "nan", "-Infinity");
}

TEST_F(AndTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "nan", "-Infinity");
}

TEST_F(AndTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, false, "nan", "NaN");
}

TEST_F(AndTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::And, true, "nan", "NaN");
}
