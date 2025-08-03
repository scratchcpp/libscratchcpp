#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMStringEqualCaseInsensitiveTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 10, 10);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 10, 10);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 10, 8);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 10, 8);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentIntegersReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 8, 10);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentIntegersReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 8, 10);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, -4.25, -4.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, -4.25, -4.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, -4.25, 5.312);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, -4.25, 5.312);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentDecimalsReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5.312, -4.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentDecimalsReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5.312, -4.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentBooleansReversed)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentBooleansReversed_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 1, true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 1, true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 1, false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 1, false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "abC def", "abC def");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "abC def", "abC def");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "abC def", "abc dEf");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "abC def", "abc dEf");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "abC def", "ghi Jkl");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "abC def", "ghi Jkl");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStringsLonger)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "abC def", "hello world");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStringsLonger_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "abC def", "hello world");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SpaceVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, " ", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SpaceVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, " ", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SpaceVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, " ", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SpaceVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, " ", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SpaceVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, " ", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, SpaceVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, " ", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroNumberVsSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroNumberVsSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, EmptyVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, EmptyVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, EmptyVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, EmptyVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroNumberVsEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroNumberVsEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroStringVsZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "0", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroStringVsZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "0", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroNumberVsZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroNumberVsZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsExactString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5.25, "5.25");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5.25, "5.25");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ExactStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ExactStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5.25, " 5.25");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5.25, " 5.25");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithLeadingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, " 5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithLeadingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, " 5.25", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5.25, "5.25 ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5.25, "5.25 ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithTrailingSpaceVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithTrailingSpaceVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5.25, " 5.25 ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5.25, " 5.25 ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithSurroundingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, " 5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithSurroundingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, " 5.25 ", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5.25, "5.26");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5.25, "5.26");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStringVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "5.26", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStringVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "5.26", 5.25);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "5.25", "5.26");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "5.25", "5.26");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5, "5  ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberVsStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5, "5  ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithTrailingSpacesVsNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "5  ", 5);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, StringWithTrailingSpacesVsNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "5  ", 5);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroVsOneString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, "1");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroVsOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, "1");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, OneStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "1", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, OneStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "1", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroVsTestString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, "test");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroVsTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, "test");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TestStringVsZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "test", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TestStringVsZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "test", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5, "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5, "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5, "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5, "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 5, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NumberWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 5, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "true", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "true", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "false", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "false", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "true", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "true", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "false", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "false", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "TRUE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "TRUE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "TRUE", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "TRUE", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, "FALSE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, "FALSE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "FALSE", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "FALSE", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "00001");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "00001");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "00001", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "00001", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "00000");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "00000");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "00000", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "00000", true);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, "00000");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, "00000");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "00000", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "00000", false);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "true", 1);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "true", 1);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 1, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 1, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "true", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "true", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, "true");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "false", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "false", 0);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 0, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 0, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "false", 1);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "false", 1);
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, 1, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, 1, "false");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "true", "TRUE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "true", "TRUE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "true", "FALSE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "true", "FALSE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "false", "FALSE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "false", "FALSE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "false", "TRUE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "false", "TRUE");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, true, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, true, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, false, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, false, "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-infinity", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-infinity", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-infinity", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "nan", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "nan", "Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "nan", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "nan", "-Infinity");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "nan", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "nan", "NaN");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "Infinity", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "Infinity", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "abc");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "-Infinity", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "-Infinity", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "abc");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "abc");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", " ");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, false, "NaN", "0");
}

TEST_F(LLVMStringEqualCaseInsensitiveTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::StrCmpEQCI, true, "NaN", "0");
}
