#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMAndTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMAndTest, NumberWithNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 10, 8);
}

TEST_F(LLVMAndTest, NumberWithNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 10, 8);
}

TEST_F(LLVMAndTest, SameNegativeDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, -4.25, -4.25);
}

TEST_F(LLVMAndTest, SameNegativeDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, -4.25, -4.25);
}

TEST_F(LLVMAndTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, -4.25, 5.312);
}

TEST_F(LLVMAndTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, -4.25, 5.312);
}

TEST_F(LLVMAndTest, TrueAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, true);
}

TEST_F(LLVMAndTest, TrueAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, true);
}

TEST_F(LLVMAndTest, TrueAndFalse)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, false);
}

TEST_F(LLVMAndTest, TrueAndFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, false);
}

TEST_F(LLVMAndTest, FalseAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, true);
}

TEST_F(LLVMAndTest, FalseAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, true);
}

TEST_F(LLVMAndTest, NumberAndTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 1, true);
}

TEST_F(LLVMAndTest, NumberAndTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 1, true);
}

TEST_F(LLVMAndTest, NumberAndFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 1, false);
}

TEST_F(LLVMAndTest, NumberAndFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 1, false);
}

TEST_F(LLVMAndTest, StringAndString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "abc", "def");
}

TEST_F(LLVMAndTest, StringAndString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "abc", "def");
}

TEST_F(LLVMAndTest, TrueStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", "true");
}

TEST_F(LLVMAndTest, TrueStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", "true");
}

TEST_F(LLVMAndTest, TrueStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", "false");
}

TEST_F(LLVMAndTest, TrueStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", "false");
}

TEST_F(LLVMAndTest, FalseStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", "true");
}

TEST_F(LLVMAndTest, FalseStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", "true");
}

TEST_F(LLVMAndTest, FalseStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", "false");
}

TEST_F(LLVMAndTest, FalseStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", "false");
}

TEST_F(LLVMAndTest, NumberAndExactString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 5.25, "5.25");
}

TEST_F(LLVMAndTest, NumberAndExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 5.25, "5.25");
}

TEST_F(LLVMAndTest, ExactStringAndNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "5.25", 5.25);
}

TEST_F(LLVMAndTest, ExactStringAndNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "5.25", 5.25);
}

TEST_F(LLVMAndTest, ZeroAndOneString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 0, "1");
}

TEST_F(LLVMAndTest, ZeroAndOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 0, "1");
}

TEST_F(LLVMAndTest, OneStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "1", 0);
}

TEST_F(LLVMAndTest, OneStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "1", 0);
}

TEST_F(LLVMAndTest, ZeroAndTestString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 0, "test");
}

TEST_F(LLVMAndTest, ZeroAndTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 0, "test");
}

TEST_F(LLVMAndTest, TestStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "test", 0);
}

TEST_F(LLVMAndTest, TestStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "test", 0);
}

TEST_F(LLVMAndTest, InfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", "Infinity");
}

TEST_F(LLVMAndTest, InfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", "Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, NaNAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", "NaN");
}

TEST_F(LLVMAndTest, NaNAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", "NaN");
}

TEST_F(LLVMAndTest, InfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, InfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", "Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", "Infinity");
}

TEST_F(LLVMAndTest, InfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", "NaN");
}

TEST_F(LLVMAndTest, InfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", "NaN");
}

TEST_F(LLVMAndTest, NaNAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", "Infinity");
}

TEST_F(LLVMAndTest, NaNAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", "Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", "NaN");
}

TEST_F(LLVMAndTest, NegativeInfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", "NaN");
}

TEST_F(LLVMAndTest, NaNAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", "-Infinity");
}

TEST_F(LLVMAndTest, NaNAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", "-Infinity");
}

TEST_F(LLVMAndTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "true");
}

TEST_F(LLVMAndTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "true");
}

TEST_F(LLVMAndTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", true);
}

TEST_F(LLVMAndTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", true);
}

TEST_F(LLVMAndTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, "false");
}

TEST_F(LLVMAndTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, "false");
}

TEST_F(LLVMAndTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", false);
}

TEST_F(LLVMAndTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", false);
}

TEST_F(LLVMAndTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, "true");
}

TEST_F(LLVMAndTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, "true");
}

TEST_F(LLVMAndTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", false);
}

TEST_F(LLVMAndTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", false);
}

TEST_F(LLVMAndTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "false");
}

TEST_F(LLVMAndTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "false");
}

TEST_F(LLVMAndTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", true);
}

TEST_F(LLVMAndTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", true);
}

TEST_F(LLVMAndTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "TRUE");
}

TEST_F(LLVMAndTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "TRUE");
}

TEST_F(LLVMAndTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "TRUE", true);
}

TEST_F(LLVMAndTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "TRUE", true);
}

TEST_F(LLVMAndTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, "FALSE");
}

TEST_F(LLVMAndTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, "FALSE");
}

TEST_F(LLVMAndTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "FALSE", false);
}

TEST_F(LLVMAndTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "FALSE", false);
}

TEST_F(LLVMAndTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "00001");
}

TEST_F(LLVMAndTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "00001");
}

TEST_F(LLVMAndTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "00001", true);
}

TEST_F(LLVMAndTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "00001", true);
}

TEST_F(LLVMAndTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "00000");
}

TEST_F(LLVMAndTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "00000");
}

TEST_F(LLVMAndTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "00000", true);
}

TEST_F(LLVMAndTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "00000", true);
}

TEST_F(LLVMAndTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, "00000");
}

TEST_F(LLVMAndTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, "00000");
}

TEST_F(LLVMAndTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "00000", false);
}

TEST_F(LLVMAndTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "00000", false);
}

TEST_F(LLVMAndTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", 1);
}

TEST_F(LLVMAndTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", 1);
}

TEST_F(LLVMAndTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 1, "true");
}

TEST_F(LLVMAndTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 1, "true");
}

TEST_F(LLVMAndTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", 0);
}

TEST_F(LLVMAndTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", 0);
}

TEST_F(LLVMAndTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 0, "true");
}

TEST_F(LLVMAndTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 0, "true");
}

TEST_F(LLVMAndTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", 0);
}

TEST_F(LLVMAndTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", 0);
}

TEST_F(LLVMAndTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 0, "false");
}

TEST_F(LLVMAndTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 0, "false");
}

TEST_F(LLVMAndTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", 1);
}

TEST_F(LLVMAndTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", 1);
}

TEST_F(LLVMAndTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, 1, "false");
}

TEST_F(LLVMAndTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, 1, "false");
}

TEST_F(LLVMAndTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", "TRUE");
}

TEST_F(LLVMAndTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", "TRUE");
}

TEST_F(LLVMAndTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "true", "FALSE");
}

TEST_F(LLVMAndTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "true", "FALSE");
}

TEST_F(LLVMAndTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", "FALSE");
}

TEST_F(LLVMAndTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", "FALSE");
}

TEST_F(LLVMAndTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "false", "TRUE");
}

TEST_F(LLVMAndTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "false", "TRUE");
}

TEST_F(LLVMAndTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "Infinity");
}

TEST_F(LLVMAndTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "Infinity");
}

TEST_F(LLVMAndTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", true);
}

TEST_F(LLVMAndTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", true);
}

TEST_F(LLVMAndTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "-Infinity");
}

TEST_F(LLVMAndTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "-Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", true);
}

TEST_F(LLVMAndTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", true);
}

TEST_F(LLVMAndTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, true, "NaN");
}

TEST_F(LLVMAndTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, true, "NaN");
}

TEST_F(LLVMAndTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", true);
}

TEST_F(LLVMAndTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", true);
}

TEST_F(LLVMAndTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, "Infinity");
}

TEST_F(LLVMAndTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, "Infinity");
}

TEST_F(LLVMAndTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", false);
}

TEST_F(LLVMAndTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", false);
}

TEST_F(LLVMAndTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, "-Infinity");
}

TEST_F(LLVMAndTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, "-Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", false);
}

TEST_F(LLVMAndTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", false);
}

TEST_F(LLVMAndTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, false, "NaN");
}

TEST_F(LLVMAndTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, false, "NaN");
}

TEST_F(LLVMAndTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", false);
}

TEST_F(LLVMAndTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", false);
}

TEST_F(LLVMAndTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", "Infinity");
}

TEST_F(LLVMAndTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", "Infinity");
}

TEST_F(LLVMAndTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "Infinity", "NaN");
}

TEST_F(LLVMAndTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "Infinity", "NaN");
}

TEST_F(LLVMAndTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "infinity", "Infinity");
}

TEST_F(LLVMAndTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "infinity", "Infinity");
}

TEST_F(LLVMAndTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "infinity", "-Infinity");
}

TEST_F(LLVMAndTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "infinity", "-Infinity");
}

TEST_F(LLVMAndTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "infinity", "NaN");
}

TEST_F(LLVMAndTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "infinity", "NaN");
}

TEST_F(LLVMAndTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", "Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", "Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMAndTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-Infinity", "NaN");
}

TEST_F(LLVMAndTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-Infinity", "NaN");
}

TEST_F(LLVMAndTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-infinity", "Infinity");
}

TEST_F(LLVMAndTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-infinity", "Infinity");
}

TEST_F(LLVMAndTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-infinity", "-Infinity");
}

TEST_F(LLVMAndTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-infinity", "-Infinity");
}

TEST_F(LLVMAndTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "-infinity", "NaN");
}

TEST_F(LLVMAndTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "-infinity", "NaN");
}

TEST_F(LLVMAndTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", "Infinity");
}

TEST_F(LLVMAndTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", "Infinity");
}

TEST_F(LLVMAndTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", "-Infinity");
}

TEST_F(LLVMAndTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", "-Infinity");
}

TEST_F(LLVMAndTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "NaN", "NaN");
}

TEST_F(LLVMAndTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "NaN", "NaN");
}

TEST_F(LLVMAndTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "nan", "Infinity");
}

TEST_F(LLVMAndTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "nan", "Infinity");
}

TEST_F(LLVMAndTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "nan", "-Infinity");
}

TEST_F(LLVMAndTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "nan", "-Infinity");
}

TEST_F(LLVMAndTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, false, "nan", "NaN");
}

TEST_F(LLVMAndTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::And, true, "nan", "NaN");
}
