#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class StringEqualCaseSensitiveTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(StringEqualCaseSensitiveTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 10, 10);
}

TEST_F(StringEqualCaseSensitiveTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 10, 10);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 10, 8);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 10, 8);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentIntegersReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 8, 10);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentIntegersReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 8, 10);
}

TEST_F(StringEqualCaseSensitiveTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, -4.25, -4.25);
}

TEST_F(StringEqualCaseSensitiveTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, -4.25, -4.25);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, -4.25, 5.312);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, -4.25, 5.312);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentDecimalsReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5.312, -4.25);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentDecimalsReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5.312, -4.25);
}

TEST_F(StringEqualCaseSensitiveTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, true);
}

TEST_F(StringEqualCaseSensitiveTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, true);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, false);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, false);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentBooleansReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, true);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentBooleansReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, true);
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 1, true);
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 1, true);
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 1, false);
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 1, false);
}

TEST_F(StringEqualCaseSensitiveTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "abC def", "abC def");
}

TEST_F(StringEqualCaseSensitiveTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "abC def", "abC def");
}

TEST_F(StringEqualCaseSensitiveTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "abC def", "abc dEf");
}

TEST_F(StringEqualCaseSensitiveTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "abC def", "abc dEf");
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "abC def", "ghi Jkl");
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "abC def", "ghi Jkl");
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStringsLonger)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "abC def", "hello world");
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStringsLonger_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "abC def", "hello world");
}

TEST_F(StringEqualCaseSensitiveTest, SpaceVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, " ", "");
}

TEST_F(StringEqualCaseSensitiveTest, SpaceVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, " ", "");
}

TEST_F(StringEqualCaseSensitiveTest, SpaceVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, " ", "0");
}

TEST_F(StringEqualCaseSensitiveTest, SpaceVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, " ", "0");
}

TEST_F(StringEqualCaseSensitiveTest, SpaceVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, " ", 0);
}

TEST_F(StringEqualCaseSensitiveTest, SpaceVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, " ", 0);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroNumberVsSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, " ");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroNumberVsSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, " ");
}

TEST_F(StringEqualCaseSensitiveTest, EmptyVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "", "0");
}

TEST_F(StringEqualCaseSensitiveTest, EmptyVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "", "0");
}

TEST_F(StringEqualCaseSensitiveTest, EmptyVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "", 0);
}

TEST_F(StringEqualCaseSensitiveTest, EmptyVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "", 0);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroNumberVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, "");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroNumberVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, "");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroStringVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "0", 0);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroStringVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "0", 0);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroNumberVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, "0");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroNumberVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, "0");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsExactString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5.25, "5.25");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5.25, "5.25");
}

TEST_F(StringEqualCaseSensitiveTest, ExactStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "5.25", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, ExactStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "5.25", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5.25, " 5.25");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5.25, " 5.25");
}

TEST_F(StringEqualCaseSensitiveTest, StringWithLeadingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, " 5.25", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, StringWithLeadingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, " 5.25", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5.25, "5.25 ");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5.25, "5.25 ");
}

TEST_F(StringEqualCaseSensitiveTest, StringWithTrailingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "5.25 ", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, StringWithTrailingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "5.25 ", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5.25, " 5.25 ");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5.25, " 5.25 ");
}

TEST_F(StringEqualCaseSensitiveTest, StringWithSurroundingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, " 5.25 ", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, StringWithSurroundingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, " 5.25 ", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5.25, "5.26");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5.25, "5.26");
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "5.26", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "5.26", 5.25);
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "5.25", "5.26");
}

TEST_F(StringEqualCaseSensitiveTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "5.25", "5.26");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5, "5  ");
}

TEST_F(StringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5, "5  ");
}

TEST_F(StringEqualCaseSensitiveTest, StringWithTrailingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "5  ", 5);
}

TEST_F(StringEqualCaseSensitiveTest, StringWithTrailingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "5  ", 5);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroVsOneString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, "1");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroVsOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, "1");
}

TEST_F(StringEqualCaseSensitiveTest, OneStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "1", 0);
}

TEST_F(StringEqualCaseSensitiveTest, OneStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "1", 0);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroVsTestString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, "test");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroVsTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, "test");
}

TEST_F(StringEqualCaseSensitiveTest, TestStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "test", 0);
}

TEST_F(StringEqualCaseSensitiveTest, TestStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "test", 0);
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5, "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5, "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5, "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5, "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 5, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NumberWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 5, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "true");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "true");
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "true", true);
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "true", true);
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, "false");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, "false");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "false", false);
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "false", false);
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, "true");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, "true");
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "true", false);
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "true", false);
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "false");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "false");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "false", true);
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "false", true);
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "TRUE");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "TRUE");
}

TEST_F(StringEqualCaseSensitiveTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "TRUE", true);
}

TEST_F(StringEqualCaseSensitiveTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "TRUE", true);
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, "FALSE");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, "FALSE");
}

TEST_F(StringEqualCaseSensitiveTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "FALSE", false);
}

TEST_F(StringEqualCaseSensitiveTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "FALSE", false);
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "00001");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "00001");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "00001", true);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "00001", true);
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "00000");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "00000");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "00000", true);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "00000", true);
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, "00000");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, "00000");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "00000", false);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "00000", false);
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "true", 1);
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "true", 1);
}

TEST_F(StringEqualCaseSensitiveTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 1, "true");
}

TEST_F(StringEqualCaseSensitiveTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 1, "true");
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "true", 0);
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "true", 0);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, "true");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, "true");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "false", 0);
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "false", 0);
}

TEST_F(StringEqualCaseSensitiveTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 0, "false");
}

TEST_F(StringEqualCaseSensitiveTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 0, "false");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "false", 1);
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "false", 1);
}

TEST_F(StringEqualCaseSensitiveTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, 1, "false");
}

TEST_F(StringEqualCaseSensitiveTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, 1, "false");
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "true", "TRUE");
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "true", "TRUE");
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "true", "FALSE");
}

TEST_F(StringEqualCaseSensitiveTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "true", "FALSE");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "false", "FALSE");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "false", "FALSE");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "false", "TRUE");
}

TEST_F(StringEqualCaseSensitiveTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "false", "TRUE");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, true, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, true, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, false, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, false, "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-infinity", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-infinity", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-infinity", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "nan", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "nan", "Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "nan", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "nan", "-Infinity");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "nan", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "nan", "NaN");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "abc");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "abc");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", " ");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", " ");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "Infinity", "0");
}

TEST_F(StringEqualCaseSensitiveTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "Infinity", "0");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "abc");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "abc");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", " ");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", " ");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "0");
}

TEST_F(StringEqualCaseSensitiveTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "0");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "abc");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "abc");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", " ");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", " ");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, false, "NaN", "0");
}

TEST_F(StringEqualCaseSensitiveTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCS, true, "NaN", "0");
}
