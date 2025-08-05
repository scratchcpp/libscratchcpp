#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMStringEqualCaseSensitiveTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMStringEqualCaseSensitiveTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 10, 10);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 10, 10);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 10, 8);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 10, 8);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentIntegersReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 8, 10);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentIntegersReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 8, 10);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, -4.25, -4.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, -4.25, -4.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, -4.25, 5.312);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, -4.25, 5.312);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentDecimalsReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5.312, -4.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentDecimalsReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5.312, -4.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentBooleansReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentBooleansReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 1, true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 1, true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 1, false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 1, false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "abC def", "abC def");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "abC def", "abC def");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "abC def", "abc dEf");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "abC def", "abc dEf");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "abC def", "ghi Jkl");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "abC def", "ghi Jkl");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStringsLonger)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "abC def", "hello world");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStringsLonger_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "abC def", "hello world");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SpaceVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, " ", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SpaceVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, " ", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SpaceVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, " ", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SpaceVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, " ", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SpaceVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, " ", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, SpaceVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, " ", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroNumberVsSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroNumberVsSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, EmptyVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, EmptyVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, EmptyVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, EmptyVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroNumberVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroNumberVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroStringVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "0", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroStringVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "0", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroNumberVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroNumberVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsExactString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5.25, "5.25");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5.25, "5.25");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ExactStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ExactStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5.25, " 5.25");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5.25, " 5.25");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithLeadingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, " 5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithLeadingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, " 5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5.25, "5.25 ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5.25, "5.25 ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithTrailingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithTrailingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5.25, " 5.25 ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5.25, " 5.25 ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithSurroundingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, " 5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithSurroundingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, " 5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5.25, "5.26");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5.25, "5.26");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "5.26", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "5.26", 5.25);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "5.25", "5.26");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "5.25", "5.26");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5, "5  ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberVsStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5, "5  ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithTrailingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "5  ", 5);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, StringWithTrailingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "5  ", 5);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroVsOneString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, "1");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroVsOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, "1");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, OneStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "1", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, OneStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "1", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroVsTestString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, "test");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroVsTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, "test");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TestStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "test", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TestStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "test", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5, "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5, "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5, "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5, "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 5, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NumberWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 5, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "true", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "true", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "false", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "false", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "true", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "true", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "false", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "false", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "TRUE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "TRUE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "TRUE", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "TRUE", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, "FALSE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, "FALSE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "FALSE", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "FALSE", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "00001");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "00001");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "00001", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "00001", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "00000");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "00000");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "00000", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "00000", true);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, "00000");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, "00000");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "00000", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "00000", false);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "true", 1);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "true", 1);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 1, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 1, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "true", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "true", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, "true");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "false", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "false", 0);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 0, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 0, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "false", 1);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "false", 1);
}

TEST_F(LLVMStringEqualCaseSensitiveTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, 1, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, 1, "false");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "true", "TRUE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "true", "TRUE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "true", "FALSE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "true", "FALSE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "false", "FALSE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "false", "FALSE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "false", "TRUE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "false", "TRUE");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, true, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, true, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, false, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, false, "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "nan", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "nan", "Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "nan", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "nan", "-Infinity");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "nan", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "nan", "NaN");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "Infinity", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "Infinity", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "-Infinity", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "-Infinity", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "abc");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "abc");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", " ");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, false, "NaN", "0");
}

TEST_F(LLVMStringEqualCaseSensitiveTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCS, true, "NaN", "0");
}
