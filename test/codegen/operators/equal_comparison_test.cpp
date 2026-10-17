#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class EqualComparisonTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(EqualComparisonTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 10, 10);
}

TEST_F(EqualComparisonTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 10, 10);
}

TEST_F(EqualComparisonTest, DifferentIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 10, 8);
}

TEST_F(EqualComparisonTest, DifferentIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 10, 8);
}

TEST_F(EqualComparisonTest, DifferentIntegersReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 8, 10);
}

TEST_F(EqualComparisonTest, DifferentIntegersReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 8, 10);
}

TEST_F(EqualComparisonTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, -4.25, -4.25);
}

TEST_F(EqualComparisonTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, -4.25, -4.25);
}

TEST_F(EqualComparisonTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, -4.25, 5.312);
}

TEST_F(EqualComparisonTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, -4.25, 5.312);
}

TEST_F(EqualComparisonTest, DifferentDecimalsReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5.312, -4.25);
}

TEST_F(EqualComparisonTest, DifferentDecimalsReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5.312, -4.25);
}

TEST_F(EqualComparisonTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, true);
}

TEST_F(EqualComparisonTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, true);
}

TEST_F(EqualComparisonTest, DifferentBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, false);
}

TEST_F(EqualComparisonTest, DifferentBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, false);
}

TEST_F(EqualComparisonTest, DifferentBooleansReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, true);
}

TEST_F(EqualComparisonTest, DifferentBooleansReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, true);
}

TEST_F(EqualComparisonTest, NumberWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 1, true);
}

TEST_F(EqualComparisonTest, NumberWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 1, true);
}

TEST_F(EqualComparisonTest, NumberWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 1, false);
}

TEST_F(EqualComparisonTest, NumberWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 1, false);
}

TEST_F(EqualComparisonTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "abC def", "abC def");
}

TEST_F(EqualComparisonTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "abC def", "abC def");
}

TEST_F(EqualComparisonTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "abC def", "abc dEf");
}

TEST_F(EqualComparisonTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "abC def", "abc dEf");
}

TEST_F(EqualComparisonTest, DifferentStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "abC def", "ghi Jkl");
}

TEST_F(EqualComparisonTest, DifferentStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "abC def", "ghi Jkl");
}

TEST_F(EqualComparisonTest, DifferentStringsLonger)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "abC def", "hello world");
}

TEST_F(EqualComparisonTest, DifferentStringsLonger_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "abC def", "hello world");
}

TEST_F(EqualComparisonTest, SpaceVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, " ", "");
}

TEST_F(EqualComparisonTest, SpaceVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, " ", "");
}

TEST_F(EqualComparisonTest, SpaceVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, " ", "0");
}

TEST_F(EqualComparisonTest, SpaceVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, " ", "0");
}

TEST_F(EqualComparisonTest, SpaceVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, " ", 0);
}

TEST_F(EqualComparisonTest, SpaceVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, " ", 0);
}

TEST_F(EqualComparisonTest, ZeroNumberVsSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, " ");
}

TEST_F(EqualComparisonTest, ZeroNumberVsSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, " ");
}

TEST_F(EqualComparisonTest, EmptyVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "", "0");
}

TEST_F(EqualComparisonTest, EmptyVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "", "0");
}

TEST_F(EqualComparisonTest, EmptyVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "", 0);
}

TEST_F(EqualComparisonTest, EmptyVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "", 0);
}

TEST_F(EqualComparisonTest, ZeroNumberVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, "");
}

TEST_F(EqualComparisonTest, ZeroNumberVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, "");
}

TEST_F(EqualComparisonTest, ZeroStringVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "0", 0);
}

TEST_F(EqualComparisonTest, ZeroStringVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "0", 0);
}

TEST_F(EqualComparisonTest, ZeroNumberVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, "0");
}

TEST_F(EqualComparisonTest, ZeroNumberVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, "0");
}

TEST_F(EqualComparisonTest, NumberVsExactString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5.25, "5.25");
}

TEST_F(EqualComparisonTest, NumberVsExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5.25, "5.25");
}

TEST_F(EqualComparisonTest, ExactStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "5.25", 5.25);
}

TEST_F(EqualComparisonTest, ExactStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "5.25", 5.25);
}

TEST_F(EqualComparisonTest, NumberVsStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5.25, " 5.25");
}

TEST_F(EqualComparisonTest, NumberVsStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5.25, " 5.25");
}

TEST_F(EqualComparisonTest, StringWithLeadingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, " 5.25", 5.25);
}

TEST_F(EqualComparisonTest, StringWithLeadingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, " 5.25", 5.25);
}

TEST_F(EqualComparisonTest, NumberVsStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5.25, "5.25 ");
}

TEST_F(EqualComparisonTest, NumberVsStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5.25, "5.25 ");
}

TEST_F(EqualComparisonTest, StringWithTrailingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "5.25 ", 5.25);
}

TEST_F(EqualComparisonTest, StringWithTrailingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "5.25 ", 5.25);
}

TEST_F(EqualComparisonTest, NumberVsStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5.25, " 5.25 ");
}

TEST_F(EqualComparisonTest, NumberVsStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5.25, " 5.25 ");
}

TEST_F(EqualComparisonTest, StringWithSurroundingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, " 5.25 ", 5.25);
}

TEST_F(EqualComparisonTest, StringWithSurroundingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, " 5.25 ", 5.25);
}

TEST_F(EqualComparisonTest, NumberVsDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5.25, "5.26");
}

TEST_F(EqualComparisonTest, NumberVsDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5.25, "5.26");
}

TEST_F(EqualComparisonTest, DifferentStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "5.26", 5.25);
}

TEST_F(EqualComparisonTest, DifferentStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "5.26", 5.25);
}

TEST_F(EqualComparisonTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "5.25", "5.26");
}

TEST_F(EqualComparisonTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "5.25", "5.26");
}

TEST_F(EqualComparisonTest, NumberVsStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5, "5  ");
}

TEST_F(EqualComparisonTest, NumberVsStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5, "5  ");
}

TEST_F(EqualComparisonTest, StringWithTrailingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "5  ", 5);
}

TEST_F(EqualComparisonTest, StringWithTrailingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "5  ", 5);
}

TEST_F(EqualComparisonTest, ZeroVsOneString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, "1");
}

TEST_F(EqualComparisonTest, ZeroVsOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, "1");
}

TEST_F(EqualComparisonTest, OneStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "1", 0);
}

TEST_F(EqualComparisonTest, OneStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "1", 0);
}

TEST_F(EqualComparisonTest, ZeroVsTestString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, "test");
}

TEST_F(EqualComparisonTest, ZeroVsTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, "test");
}

TEST_F(EqualComparisonTest, TestStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "test", 0);
}

TEST_F(EqualComparisonTest, TestStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "test", 0);
}

TEST_F(EqualComparisonTest, InfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, InfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, NaNWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "NaN");
}

TEST_F(EqualComparisonTest, NaNWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "NaN");
}

TEST_F(EqualComparisonTest, InfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, InfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, InfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "NaN");
}

TEST_F(EqualComparisonTest, InfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "NaN");
}

TEST_F(EqualComparisonTest, NaNWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "Infinity");
}

TEST_F(EqualComparisonTest, NaNWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "NaN");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "NaN");
}

TEST_F(EqualComparisonTest, NaNWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "-Infinity");
}

TEST_F(EqualComparisonTest, NaNWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "-Infinity");
}

TEST_F(EqualComparisonTest, NumberWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5, "Infinity");
}

TEST_F(EqualComparisonTest, NumberWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5, "Infinity");
}

TEST_F(EqualComparisonTest, NumberWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5, "-Infinity");
}

TEST_F(EqualComparisonTest, NumberWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5, "-Infinity");
}

TEST_F(EqualComparisonTest, NumberWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 5, "NaN");
}

TEST_F(EqualComparisonTest, NumberWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 5, "NaN");
}

TEST_F(EqualComparisonTest, ZeroWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, "NaN");
}

TEST_F(EqualComparisonTest, ZeroWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, "NaN");
}

TEST_F(EqualComparisonTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "true");
}

TEST_F(EqualComparisonTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "true");
}

TEST_F(EqualComparisonTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "true", true);
}

TEST_F(EqualComparisonTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, "false");
}

TEST_F(EqualComparisonTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, "false");
}

TEST_F(EqualComparisonTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "false", false);
}

TEST_F(EqualComparisonTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "false", false);
}

TEST_F(EqualComparisonTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, "true");
}

TEST_F(EqualComparisonTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, "true");
}

TEST_F(EqualComparisonTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "true", false);
}

TEST_F(EqualComparisonTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "true", false);
}

TEST_F(EqualComparisonTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "false");
}

TEST_F(EqualComparisonTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "false");
}

TEST_F(EqualComparisonTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "false", true);
}

TEST_F(EqualComparisonTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "false", true);
}

TEST_F(EqualComparisonTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "TRUE");
}

TEST_F(EqualComparisonTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "TRUE");
}

TEST_F(EqualComparisonTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "TRUE", true);
}

TEST_F(EqualComparisonTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "TRUE", true);
}

TEST_F(EqualComparisonTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, "FALSE");
}

TEST_F(EqualComparisonTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, "FALSE");
}

TEST_F(EqualComparisonTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "FALSE", false);
}

TEST_F(EqualComparisonTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "FALSE", false);
}

TEST_F(EqualComparisonTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "00001");
}

TEST_F(EqualComparisonTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "00001");
}

TEST_F(EqualComparisonTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "00001", true);
}

TEST_F(EqualComparisonTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "00001", true);
}

TEST_F(EqualComparisonTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "00000");
}

TEST_F(EqualComparisonTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "00000");
}

TEST_F(EqualComparisonTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "00000", true);
}

TEST_F(EqualComparisonTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "00000", true);
}

TEST_F(EqualComparisonTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, "00000");
}

TEST_F(EqualComparisonTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, "00000");
}

TEST_F(EqualComparisonTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "00000", false);
}

TEST_F(EqualComparisonTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "00000", false);
}

TEST_F(EqualComparisonTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "true", 1);
}

TEST_F(EqualComparisonTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "true", 1);
}

TEST_F(EqualComparisonTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 1, "true");
}

TEST_F(EqualComparisonTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 1, "true");
}

TEST_F(EqualComparisonTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "true", 0);
}

TEST_F(EqualComparisonTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "true", 0);
}

TEST_F(EqualComparisonTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, "true");
}

TEST_F(EqualComparisonTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, "true");
}

TEST_F(EqualComparisonTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "false", 0);
}

TEST_F(EqualComparisonTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "false", 0);
}

TEST_F(EqualComparisonTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 0, "false");
}

TEST_F(EqualComparisonTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 0, "false");
}

TEST_F(EqualComparisonTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "false", 1);
}

TEST_F(EqualComparisonTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "false", 1);
}

TEST_F(EqualComparisonTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, 1, "false");
}

TEST_F(EqualComparisonTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, 1, "false");
}

TEST_F(EqualComparisonTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "true", "TRUE");
}

TEST_F(EqualComparisonTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "true", "TRUE");
}

TEST_F(EqualComparisonTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "true", "FALSE");
}

TEST_F(EqualComparisonTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "true", "FALSE");
}

TEST_F(EqualComparisonTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "false", "FALSE");
}

TEST_F(EqualComparisonTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "false", "FALSE");
}

TEST_F(EqualComparisonTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "false", "TRUE");
}

TEST_F(EqualComparisonTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "false", "TRUE");
}

TEST_F(EqualComparisonTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "Infinity");
}

TEST_F(EqualComparisonTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "Infinity");
}

TEST_F(EqualComparisonTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "-Infinity");
}

TEST_F(EqualComparisonTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "-Infinity");
}

TEST_F(EqualComparisonTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, true, "NaN");
}

TEST_F(EqualComparisonTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, true, "NaN");
}

TEST_F(EqualComparisonTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, "Infinity");
}

TEST_F(EqualComparisonTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, "Infinity");
}

TEST_F(EqualComparisonTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, "-Infinity");
}

TEST_F(EqualComparisonTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, "-Infinity");
}

TEST_F(EqualComparisonTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, false, "NaN");
}

TEST_F(EqualComparisonTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, false, "NaN");
}

TEST_F(EqualComparisonTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "NaN");
}

TEST_F(EqualComparisonTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "NaN");
}

TEST_F(EqualComparisonTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "infinity", "Infinity");
}

TEST_F(EqualComparisonTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "infinity", "Infinity");
}

TEST_F(EqualComparisonTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "infinity", "NaN");
}

TEST_F(EqualComparisonTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "infinity", "NaN");
}

TEST_F(EqualComparisonTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "NaN");
}

TEST_F(EqualComparisonTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "NaN");
}

TEST_F(EqualComparisonTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-infinity", "Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-infinity", "Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-infinity", "-Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-infinity", "NaN");
}

TEST_F(EqualComparisonTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-infinity", "NaN");
}

TEST_F(EqualComparisonTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "Infinity");
}

TEST_F(EqualComparisonTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "Infinity");
}

TEST_F(EqualComparisonTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "-Infinity");
}

TEST_F(EqualComparisonTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "-Infinity");
}

TEST_F(EqualComparisonTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "NaN");
}

TEST_F(EqualComparisonTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "NaN");
}

TEST_F(EqualComparisonTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "nan", "Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "nan", "Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "nan", "-Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "nan", "-Infinity");
}

TEST_F(EqualComparisonTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "nan", "NaN");
}

TEST_F(EqualComparisonTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "nan", "NaN");
}

TEST_F(EqualComparisonTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "abc");
}

TEST_F(EqualComparisonTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "abc");
}

TEST_F(EqualComparisonTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", " ");
}

TEST_F(EqualComparisonTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", " ");
}

TEST_F(EqualComparisonTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "");
}

TEST_F(EqualComparisonTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "");
}

TEST_F(EqualComparisonTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "Infinity", "0");
}

TEST_F(EqualComparisonTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "Infinity", "0");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "abc");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "abc");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", " ");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", " ");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "-Infinity", "0");
}

TEST_F(EqualComparisonTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "-Infinity", "0");
}

TEST_F(EqualComparisonTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "abc");
}

TEST_F(EqualComparisonTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "abc");
}

TEST_F(EqualComparisonTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", " ");
}

TEST_F(EqualComparisonTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", " ");
}

TEST_F(EqualComparisonTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "");
}

TEST_F(EqualComparisonTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "");
}

TEST_F(EqualComparisonTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, false, "NaN", "0");
}

TEST_F(EqualComparisonTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpEQ, true, "NaN", "0");
}
