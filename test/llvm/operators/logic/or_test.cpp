#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMOrTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMOrTest, NumberWithNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 10, 8);
}

TEST_F(LLVMOrTest, NumberWithNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 10, 8);
}

TEST_F(LLVMOrTest, SameNegativeDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, -4.25, -4.25);
}

TEST_F(LLVMOrTest, SameNegativeDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, -4.25, -4.25);
}

TEST_F(LLVMOrTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, -4.25, 5.312);
}

TEST_F(LLVMOrTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, -4.25, 5.312);
}

TEST_F(LLVMOrTest, TrueAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, true);
}

TEST_F(LLVMOrTest, TrueAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, true);
}

TEST_F(LLVMOrTest, TrueAndFalse)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, false);
}

TEST_F(LLVMOrTest, TrueAndFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, false);
}

TEST_F(LLVMOrTest, FalseAndTrue)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, true);
}

TEST_F(LLVMOrTest, FalseAndTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, true);
}

TEST_F(LLVMOrTest, NumberAndTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 1, true);
}

TEST_F(LLVMOrTest, NumberAndTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 1, true);
}

TEST_F(LLVMOrTest, NumberAndFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 1, false);
}

TEST_F(LLVMOrTest, NumberAndFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 1, false);
}

TEST_F(LLVMOrTest, StringAndString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "abc", "def");
}

TEST_F(LLVMOrTest, StringAndString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "abc", "def");
}

TEST_F(LLVMOrTest, TrueStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", "true");
}

TEST_F(LLVMOrTest, TrueStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", "true");
}

TEST_F(LLVMOrTest, TrueStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", "false");
}

TEST_F(LLVMOrTest, TrueStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", "false");
}

TEST_F(LLVMOrTest, FalseStringAndTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", "true");
}

TEST_F(LLVMOrTest, FalseStringAndTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", "true");
}

TEST_F(LLVMOrTest, FalseStringAndFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", "false");
}

TEST_F(LLVMOrTest, FalseStringAndFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", "false");
}

TEST_F(LLVMOrTest, NumberAndExactString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 5.25, "5.25");
}

TEST_F(LLVMOrTest, NumberAndExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 5.25, "5.25");
}

TEST_F(LLVMOrTest, ExactStringAndNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "5.25", 5.25);
}

TEST_F(LLVMOrTest, ExactStringAndNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "5.25", 5.25);
}

TEST_F(LLVMOrTest, ZeroAndOneString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 0, "1");
}

TEST_F(LLVMOrTest, ZeroAndOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 0, "1");
}

TEST_F(LLVMOrTest, OneStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "1", 0);
}

TEST_F(LLVMOrTest, OneStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "1", 0);
}

TEST_F(LLVMOrTest, ZeroAndTestString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 0, "test");
}

TEST_F(LLVMOrTest, ZeroAndTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 0, "test");
}

TEST_F(LLVMOrTest, TestStringAndZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "test", 0);
}

TEST_F(LLVMOrTest, TestStringAndZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "test", 0);
}

TEST_F(LLVMOrTest, InfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", "Infinity");
}

TEST_F(LLVMOrTest, InfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", "Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, NaNAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", "NaN");
}

TEST_F(LLVMOrTest, NaNAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", "NaN");
}

TEST_F(LLVMOrTest, InfinityAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, InfinityAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", "Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", "Infinity");
}

TEST_F(LLVMOrTest, InfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", "NaN");
}

TEST_F(LLVMOrTest, InfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", "NaN");
}

TEST_F(LLVMOrTest, NaNAndInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", "Infinity");
}

TEST_F(LLVMOrTest, NaNAndInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", "Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityAndNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", "NaN");
}

TEST_F(LLVMOrTest, NegativeInfinityAndNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", "NaN");
}

TEST_F(LLVMOrTest, NaNAndNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", "-Infinity");
}

TEST_F(LLVMOrTest, NaNAndNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", "-Infinity");
}

TEST_F(LLVMOrTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "true");
}

TEST_F(LLVMOrTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "true");
}

TEST_F(LLVMOrTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", true);
}

TEST_F(LLVMOrTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", true);
}

TEST_F(LLVMOrTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, "false");
}

TEST_F(LLVMOrTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, "false");
}

TEST_F(LLVMOrTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", false);
}

TEST_F(LLVMOrTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", false);
}

TEST_F(LLVMOrTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, "true");
}

TEST_F(LLVMOrTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, "true");
}

TEST_F(LLVMOrTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", false);
}

TEST_F(LLVMOrTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", false);
}

TEST_F(LLVMOrTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "false");
}

TEST_F(LLVMOrTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "false");
}

TEST_F(LLVMOrTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", true);
}

TEST_F(LLVMOrTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", true);
}

TEST_F(LLVMOrTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "TRUE");
}

TEST_F(LLVMOrTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "TRUE");
}

TEST_F(LLVMOrTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "TRUE", true);
}

TEST_F(LLVMOrTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "TRUE", true);
}

TEST_F(LLVMOrTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, "FALSE");
}

TEST_F(LLVMOrTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, "FALSE");
}

TEST_F(LLVMOrTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "FALSE", false);
}

TEST_F(LLVMOrTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "FALSE", false);
}

TEST_F(LLVMOrTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "00001");
}

TEST_F(LLVMOrTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "00001");
}

TEST_F(LLVMOrTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "00001", true);
}

TEST_F(LLVMOrTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "00001", true);
}

TEST_F(LLVMOrTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "00000");
}

TEST_F(LLVMOrTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "00000");
}

TEST_F(LLVMOrTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "00000", true);
}

TEST_F(LLVMOrTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "00000", true);
}

TEST_F(LLVMOrTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, "00000");
}

TEST_F(LLVMOrTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, "00000");
}

TEST_F(LLVMOrTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "00000", false);
}

TEST_F(LLVMOrTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "00000", false);
}

TEST_F(LLVMOrTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", 1);
}

TEST_F(LLVMOrTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", 1);
}

TEST_F(LLVMOrTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 1, "true");
}

TEST_F(LLVMOrTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 1, "true");
}

TEST_F(LLVMOrTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", 0);
}

TEST_F(LLVMOrTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", 0);
}

TEST_F(LLVMOrTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 0, "true");
}

TEST_F(LLVMOrTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 0, "true");
}

TEST_F(LLVMOrTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", 0);
}

TEST_F(LLVMOrTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", 0);
}

TEST_F(LLVMOrTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 0, "false");
}

TEST_F(LLVMOrTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 0, "false");
}

TEST_F(LLVMOrTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", 1);
}

TEST_F(LLVMOrTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", 1);
}

TEST_F(LLVMOrTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, 1, "false");
}

TEST_F(LLVMOrTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, 1, "false");
}

TEST_F(LLVMOrTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", "TRUE");
}

TEST_F(LLVMOrTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", "TRUE");
}

TEST_F(LLVMOrTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "true", "FALSE");
}

TEST_F(LLVMOrTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "true", "FALSE");
}

TEST_F(LLVMOrTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", "FALSE");
}

TEST_F(LLVMOrTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", "FALSE");
}

TEST_F(LLVMOrTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "false", "TRUE");
}

TEST_F(LLVMOrTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "false", "TRUE");
}

TEST_F(LLVMOrTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "Infinity");
}

TEST_F(LLVMOrTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "Infinity");
}

TEST_F(LLVMOrTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", true);
}

TEST_F(LLVMOrTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", true);
}

TEST_F(LLVMOrTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "-Infinity");
}

TEST_F(LLVMOrTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "-Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", true);
}

TEST_F(LLVMOrTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", true);
}

TEST_F(LLVMOrTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, true, "NaN");
}

TEST_F(LLVMOrTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, true, "NaN");
}

TEST_F(LLVMOrTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", true);
}

TEST_F(LLVMOrTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", true);
}

TEST_F(LLVMOrTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, "Infinity");
}

TEST_F(LLVMOrTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, "Infinity");
}

TEST_F(LLVMOrTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", false);
}

TEST_F(LLVMOrTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", false);
}

TEST_F(LLVMOrTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, "-Infinity");
}

TEST_F(LLVMOrTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, "-Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", false);
}

TEST_F(LLVMOrTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", false);
}

TEST_F(LLVMOrTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, false, "NaN");
}

TEST_F(LLVMOrTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, false, "NaN");
}

TEST_F(LLVMOrTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", false);
}

TEST_F(LLVMOrTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", false);
}

TEST_F(LLVMOrTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", "Infinity");
}

TEST_F(LLVMOrTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", "Infinity");
}

TEST_F(LLVMOrTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "Infinity", "NaN");
}

TEST_F(LLVMOrTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "Infinity", "NaN");
}

TEST_F(LLVMOrTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "infinity", "Infinity");
}

TEST_F(LLVMOrTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "infinity", "Infinity");
}

TEST_F(LLVMOrTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "infinity", "-Infinity");
}

TEST_F(LLVMOrTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "infinity", "-Infinity");
}

TEST_F(LLVMOrTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "infinity", "NaN");
}

TEST_F(LLVMOrTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "infinity", "NaN");
}

TEST_F(LLVMOrTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", "Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", "Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMOrTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-Infinity", "NaN");
}

TEST_F(LLVMOrTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-Infinity", "NaN");
}

TEST_F(LLVMOrTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-infinity", "Infinity");
}

TEST_F(LLVMOrTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-infinity", "Infinity");
}

TEST_F(LLVMOrTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-infinity", "-Infinity");
}

TEST_F(LLVMOrTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-infinity", "-Infinity");
}

TEST_F(LLVMOrTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "-infinity", "NaN");
}

TEST_F(LLVMOrTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "-infinity", "NaN");
}

TEST_F(LLVMOrTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", "Infinity");
}

TEST_F(LLVMOrTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", "Infinity");
}

TEST_F(LLVMOrTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", "-Infinity");
}

TEST_F(LLVMOrTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", "-Infinity");
}

TEST_F(LLVMOrTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "NaN", "NaN");
}

TEST_F(LLVMOrTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "NaN", "NaN");
}

TEST_F(LLVMOrTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "nan", "Infinity");
}

TEST_F(LLVMOrTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "nan", "Infinity");
}

TEST_F(LLVMOrTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "nan", "-Infinity");
}

TEST_F(LLVMOrTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "nan", "-Infinity");
}

TEST_F(LLVMOrTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, false, "nan", "NaN");
}

TEST_F(LLVMOrTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::Or, true, "nan", "NaN");
}
