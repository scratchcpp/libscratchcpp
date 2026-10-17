#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class StringEqualCaseInsensitiveTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(StringEqualCaseInsensitiveTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 10, 10);
}

TEST_F(StringEqualCaseInsensitiveTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 10, 10);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 10, 8);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 10, 8);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentIntegersReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 8, 10);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentIntegersReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 8, 10);
}

TEST_F(StringEqualCaseInsensitiveTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, -4.25, -4.25);
}

TEST_F(StringEqualCaseInsensitiveTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, -4.25, -4.25);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, -4.25, 5.312);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, -4.25, 5.312);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentDecimalsReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5.312, -4.25);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentDecimalsReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5.312, -4.25);
}

TEST_F(StringEqualCaseInsensitiveTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, true);
}

TEST_F(StringEqualCaseInsensitiveTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, true);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, false);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, false);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentBooleansReversed)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, true);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentBooleansReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, true);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 1, true);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 1, true);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 1, false);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 1, false);
}

TEST_F(StringEqualCaseInsensitiveTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "abC def", "abC def");
}

TEST_F(StringEqualCaseInsensitiveTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "abC def", "abC def");
}

TEST_F(StringEqualCaseInsensitiveTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "abC def", "abc dEf");
}

TEST_F(StringEqualCaseInsensitiveTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "abC def", "abc dEf");
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "abC def", "ghi Jkl");
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "abC def", "ghi Jkl");
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStringsLonger)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "abC def", "hello world");
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStringsLonger_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "abC def", "hello world");
}

TEST_F(StringEqualCaseInsensitiveTest, SpaceVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, " ", "");
}

TEST_F(StringEqualCaseInsensitiveTest, SpaceVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, " ", "");
}

TEST_F(StringEqualCaseInsensitiveTest, SpaceVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, " ", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, SpaceVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, " ", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, SpaceVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, " ", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, SpaceVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, " ", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroNumberVsSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, " ");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroNumberVsSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, " ");
}

TEST_F(StringEqualCaseInsensitiveTest, EmptyVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, EmptyVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, EmptyVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, EmptyVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroNumberVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, "");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroNumberVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, "");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroStringVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "0", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroStringVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "0", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroNumberVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, "0");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroNumberVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, "0");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsExactString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5.25, "5.25");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5.25, "5.25");
}

TEST_F(StringEqualCaseInsensitiveTest, ExactStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "5.25", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, ExactStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "5.25", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5.25, " 5.25");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5.25, " 5.25");
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithLeadingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, " 5.25", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithLeadingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, " 5.25", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5.25, "5.25 ");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5.25, "5.25 ");
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithTrailingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "5.25 ", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithTrailingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "5.25 ", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5.25, " 5.25 ");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5.25, " 5.25 ");
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithSurroundingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, " 5.25 ", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithSurroundingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, " 5.25 ", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5.25, "5.26");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5.25, "5.26");
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "5.26", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "5.26", 5.25);
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "5.25", "5.26");
}

TEST_F(StringEqualCaseInsensitiveTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "5.25", "5.26");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5, "5  ");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5, "5  ");
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithTrailingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "5  ", 5);
}

TEST_F(StringEqualCaseInsensitiveTest, StringWithTrailingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "5  ", 5);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroVsOneString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, "1");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroVsOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, "1");
}

TEST_F(StringEqualCaseInsensitiveTest, OneStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "1", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, OneStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "1", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroVsTestString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, "test");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroVsTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, "test");
}

TEST_F(StringEqualCaseInsensitiveTest, TestStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "test", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, TestStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "test", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5, "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5, "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5, "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5, "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 5, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NumberWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 5, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "true", true);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "true", true);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "false", false);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "false", false);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "true", false);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "true", false);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "false", true);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "false", true);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "TRUE");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "TRUE");
}

TEST_F(StringEqualCaseInsensitiveTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "TRUE", true);
}

TEST_F(StringEqualCaseInsensitiveTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "TRUE", true);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, "FALSE");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, "FALSE");
}

TEST_F(StringEqualCaseInsensitiveTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "FALSE", false);
}

TEST_F(StringEqualCaseInsensitiveTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "FALSE", false);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "00001");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "00001");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "00001", true);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "00001", true);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "00000");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "00000");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "00000", true);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "00000", true);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, "00000");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, "00000");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "00000", false);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "00000", false);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "true", 1);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "true", 1);
}

TEST_F(StringEqualCaseInsensitiveTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 1, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 1, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "true", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "true", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, "true");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "false", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "false", 0);
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 0, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 0, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "false", 1);
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "false", 1);
}

TEST_F(StringEqualCaseInsensitiveTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, 1, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, 1, "false");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "true", "TRUE");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "true", "TRUE");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "true", "FALSE");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "true", "FALSE");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "false", "FALSE");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "false", "FALSE");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "false", "TRUE");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "false", "TRUE");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, true, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, true, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, false, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, false, "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-infinity", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-infinity", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-infinity", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "nan", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "nan", "Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "nan", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "nan", "-Infinity");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "nan", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "nan", "NaN");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "abc");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "abc");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", " ");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", " ");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "Infinity", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "Infinity", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "abc");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "abc");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", " ");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", " ");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "abc");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "abc");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", " ");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", " ");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, false, "NaN", "0");
}

TEST_F(StringEqualCaseInsensitiveTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::StrCmpEQCI, true, "NaN", "0");
}
