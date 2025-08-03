#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMEqualComparisonTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMEqualComparisonTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 10, 10);
}

TEST_F(LLVMEqualComparisonTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 10, 10);
}

TEST_F(LLVMEqualComparisonTest, DifferentIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 10, 8);
}

TEST_F(LLVMEqualComparisonTest, DifferentIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 10, 8);
}

TEST_F(LLVMEqualComparisonTest, DifferentIntegersReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 8, 10);
}

TEST_F(LLVMEqualComparisonTest, DifferentIntegersReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 8, 10);
}

TEST_F(LLVMEqualComparisonTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, -4.25, -4.25);
}

TEST_F(LLVMEqualComparisonTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, -4.25, -4.25);
}

TEST_F(LLVMEqualComparisonTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, -4.25, 5.312);
}

TEST_F(LLVMEqualComparisonTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, -4.25, 5.312);
}

TEST_F(LLVMEqualComparisonTest, DifferentDecimalsReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5.312, -4.25);
}

TEST_F(LLVMEqualComparisonTest, DifferentDecimalsReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5.312, -4.25);
}

TEST_F(LLVMEqualComparisonTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, true);
}

TEST_F(LLVMEqualComparisonTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, true);
}

TEST_F(LLVMEqualComparisonTest, DifferentBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, false);
}

TEST_F(LLVMEqualComparisonTest, DifferentBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, false);
}

TEST_F(LLVMEqualComparisonTest, DifferentBooleansReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, true);
}

TEST_F(LLVMEqualComparisonTest, DifferentBooleansReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, true);
}

TEST_F(LLVMEqualComparisonTest, NumberWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 1, true);
}

TEST_F(LLVMEqualComparisonTest, NumberWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 1, true);
}

TEST_F(LLVMEqualComparisonTest, NumberWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 1, false);
}

TEST_F(LLVMEqualComparisonTest, NumberWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 1, false);
}

TEST_F(LLVMEqualComparisonTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "abC def", "abC def");
}

TEST_F(LLVMEqualComparisonTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "abC def", "abC def");
}

TEST_F(LLVMEqualComparisonTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "abC def", "abc dEf");
}

TEST_F(LLVMEqualComparisonTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "abC def", "abc dEf");
}

TEST_F(LLVMEqualComparisonTest, DifferentStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "abC def", "ghi Jkl");
}

TEST_F(LLVMEqualComparisonTest, DifferentStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "abC def", "ghi Jkl");
}

TEST_F(LLVMEqualComparisonTest, DifferentStringsLonger)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "abC def", "hello world");
}

TEST_F(LLVMEqualComparisonTest, DifferentStringsLonger_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "abC def", "hello world");
}

TEST_F(LLVMEqualComparisonTest, SpaceVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, " ", "");
}

TEST_F(LLVMEqualComparisonTest, SpaceVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, " ", "");
}

TEST_F(LLVMEqualComparisonTest, SpaceVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, " ", "0");
}

TEST_F(LLVMEqualComparisonTest, SpaceVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, " ", "0");
}

TEST_F(LLVMEqualComparisonTest, SpaceVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, " ", 0);
}

TEST_F(LLVMEqualComparisonTest, SpaceVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, " ", 0);
}

TEST_F(LLVMEqualComparisonTest, ZeroNumberVsSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, " ");
}

TEST_F(LLVMEqualComparisonTest, ZeroNumberVsSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, " ");
}

TEST_F(LLVMEqualComparisonTest, EmptyVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "", "0");
}

TEST_F(LLVMEqualComparisonTest, EmptyVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "", "0");
}

TEST_F(LLVMEqualComparisonTest, EmptyVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "", 0);
}

TEST_F(LLVMEqualComparisonTest, EmptyVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "", 0);
}

TEST_F(LLVMEqualComparisonTest, ZeroNumberVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, "");
}

TEST_F(LLVMEqualComparisonTest, ZeroNumberVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, "");
}

TEST_F(LLVMEqualComparisonTest, ZeroStringVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "0", 0);
}

TEST_F(LLVMEqualComparisonTest, ZeroStringVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "0", 0);
}

TEST_F(LLVMEqualComparisonTest, ZeroNumberVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, "0");
}

TEST_F(LLVMEqualComparisonTest, ZeroNumberVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, "0");
}

TEST_F(LLVMEqualComparisonTest, NumberVsExactString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5.25, "5.25");
}

TEST_F(LLVMEqualComparisonTest, NumberVsExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5.25, "5.25");
}

TEST_F(LLVMEqualComparisonTest, ExactStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "5.25", 5.25);
}

TEST_F(LLVMEqualComparisonTest, ExactStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "5.25", 5.25);
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5.25, " 5.25");
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5.25, " 5.25");
}

TEST_F(LLVMEqualComparisonTest, StringWithLeadingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, " 5.25", 5.25);
}

TEST_F(LLVMEqualComparisonTest, StringWithLeadingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, " 5.25", 5.25);
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5.25, "5.25 ");
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5.25, "5.25 ");
}

TEST_F(LLVMEqualComparisonTest, StringWithTrailingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "5.25 ", 5.25);
}

TEST_F(LLVMEqualComparisonTest, StringWithTrailingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "5.25 ", 5.25);
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5.25, " 5.25 ");
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5.25, " 5.25 ");
}

TEST_F(LLVMEqualComparisonTest, StringWithSurroundingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, " 5.25 ", 5.25);
}

TEST_F(LLVMEqualComparisonTest, StringWithSurroundingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, " 5.25 ", 5.25);
}

TEST_F(LLVMEqualComparisonTest, NumberVsDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5.25, "5.26");
}

TEST_F(LLVMEqualComparisonTest, NumberVsDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5.25, "5.26");
}

TEST_F(LLVMEqualComparisonTest, DifferentStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "5.26", 5.25);
}

TEST_F(LLVMEqualComparisonTest, DifferentStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "5.26", 5.25);
}

TEST_F(LLVMEqualComparisonTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "5.25", "5.26");
}

TEST_F(LLVMEqualComparisonTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "5.25", "5.26");
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5, "5  ");
}

TEST_F(LLVMEqualComparisonTest, NumberVsStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5, "5  ");
}

TEST_F(LLVMEqualComparisonTest, StringWithTrailingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "5  ", 5);
}

TEST_F(LLVMEqualComparisonTest, StringWithTrailingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "5  ", 5);
}

TEST_F(LLVMEqualComparisonTest, ZeroVsOneString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, "1");
}

TEST_F(LLVMEqualComparisonTest, ZeroVsOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, "1");
}

TEST_F(LLVMEqualComparisonTest, OneStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "1", 0);
}

TEST_F(LLVMEqualComparisonTest, OneStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "1", 0);
}

TEST_F(LLVMEqualComparisonTest, ZeroVsTestString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, "test");
}

TEST_F(LLVMEqualComparisonTest, ZeroVsTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, "test");
}

TEST_F(LLVMEqualComparisonTest, TestStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "test", 0);
}

TEST_F(LLVMEqualComparisonTest, TestStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "test", 0);
}

TEST_F(LLVMEqualComparisonTest, InfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NaNWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NaNWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "NaN");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NaNWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NaNWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NaNWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NaNWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NumberWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5, "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NumberWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5, "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NumberWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5, "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NumberWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5, "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NumberWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 5, "NaN");
}

TEST_F(LLVMEqualComparisonTest, NumberWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 5, "NaN");
}

TEST_F(LLVMEqualComparisonTest, ZeroWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, "NaN");
}

TEST_F(LLVMEqualComparisonTest, ZeroWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, "NaN");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "true");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "true");
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "true", true);
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, "false");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, "false");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "false", false);
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "false", false);
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, "true");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, "true");
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "true", false);
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "true", false);
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "false");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "false");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "false", true);
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "false", true);
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "TRUE");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "TRUE");
}

TEST_F(LLVMEqualComparisonTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "TRUE", true);
}

TEST_F(LLVMEqualComparisonTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "TRUE", true);
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, "FALSE");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, "FALSE");
}

TEST_F(LLVMEqualComparisonTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "FALSE", false);
}

TEST_F(LLVMEqualComparisonTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "FALSE", false);
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "00001");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "00001");
}

TEST_F(LLVMEqualComparisonTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "00001", true);
}

TEST_F(LLVMEqualComparisonTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "00001", true);
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "00000");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "00000");
}

TEST_F(LLVMEqualComparisonTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "00000", true);
}

TEST_F(LLVMEqualComparisonTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "00000", true);
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, "00000");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, "00000");
}

TEST_F(LLVMEqualComparisonTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "00000", false);
}

TEST_F(LLVMEqualComparisonTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "00000", false);
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "true", 1);
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "true", 1);
}

TEST_F(LLVMEqualComparisonTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 1, "true");
}

TEST_F(LLVMEqualComparisonTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 1, "true");
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "true", 0);
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "true", 0);
}

TEST_F(LLVMEqualComparisonTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, "true");
}

TEST_F(LLVMEqualComparisonTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, "true");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "false", 0);
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "false", 0);
}

TEST_F(LLVMEqualComparisonTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 0, "false");
}

TEST_F(LLVMEqualComparisonTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 0, "false");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "false", 1);
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "false", 1);
}

TEST_F(LLVMEqualComparisonTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, 1, "false");
}

TEST_F(LLVMEqualComparisonTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, 1, "false");
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "true", "TRUE");
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "true", "TRUE");
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "true", "FALSE");
}

TEST_F(LLVMEqualComparisonTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "true", "FALSE");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "false", "FALSE");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "false", "FALSE");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "false", "TRUE");
}

TEST_F(LLVMEqualComparisonTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "false", "TRUE");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "Infinity");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "Infinity");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, true, "NaN");
}

TEST_F(LLVMEqualComparisonTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, true, "NaN");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, "Infinity");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, "Infinity");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, false, "NaN");
}

TEST_F(LLVMEqualComparisonTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, false, "NaN");
}

TEST_F(LLVMEqualComparisonTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-infinity", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-infinity", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-infinity", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "NaN");
}

TEST_F(LLVMEqualComparisonTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "NaN");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "nan", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "nan", "Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "nan", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "nan", "-Infinity");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "nan", "NaN");
}

TEST_F(LLVMEqualComparisonTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "nan", "NaN");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "abc");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "abc");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", " ");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", " ");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "Infinity", "0");
}

TEST_F(LLVMEqualComparisonTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "Infinity", "0");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "abc");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "abc");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", " ");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", " ");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "-Infinity", "0");
}

TEST_F(LLVMEqualComparisonTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "-Infinity", "0");
}

TEST_F(LLVMEqualComparisonTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "abc");
}

TEST_F(LLVMEqualComparisonTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "abc");
}

TEST_F(LLVMEqualComparisonTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", " ");
}

TEST_F(LLVMEqualComparisonTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", " ");
}

TEST_F(LLVMEqualComparisonTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "");
}

TEST_F(LLVMEqualComparisonTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "");
}

TEST_F(LLVMEqualComparisonTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, false, "NaN", "0");
}

TEST_F(LLVMEqualComparisonTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpEQ, true, "NaN", "0");
}
