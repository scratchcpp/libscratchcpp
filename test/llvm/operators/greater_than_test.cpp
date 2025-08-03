#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMGreaterThanTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMGreaterThanTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 10, 10);
}

TEST_F(LLVMGreaterThanTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 10, 10);
}

TEST_F(LLVMGreaterThanTest, FirstGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 10, 8);
}

TEST_F(LLVMGreaterThanTest, FirstGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 10, 8);
}

TEST_F(LLVMGreaterThanTest, SecondGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 8, 10);
}

TEST_F(LLVMGreaterThanTest, SecondGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 8, 10);
}

TEST_F(LLVMGreaterThanTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, -4.25, -4.25);
}

TEST_F(LLVMGreaterThanTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, -4.25, -4.25);
}

TEST_F(LLVMGreaterThanTest, FirstGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, -4.25, 5.312);
}

TEST_F(LLVMGreaterThanTest, FirstGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, -4.25, 5.312);
}

TEST_F(LLVMGreaterThanTest, SecondGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5.312, -4.25);
}

TEST_F(LLVMGreaterThanTest, SecondGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5.312, -4.25);
}

TEST_F(LLVMGreaterThanTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, true);
}

TEST_F(LLVMGreaterThanTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, true);
}

TEST_F(LLVMGreaterThanTest, TrueGreaterThanFalse)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, false);
}

TEST_F(LLVMGreaterThanTest, TrueGreaterThanFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, false);
}

TEST_F(LLVMGreaterThanTest, FalseGreaterThanTrue)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, true);
}

TEST_F(LLVMGreaterThanTest, FalseGreaterThanTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, true);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 1, true);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 1, true);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 1, false);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 1, false);
}

TEST_F(LLVMGreaterThanTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "abC def", "abC def");
}

TEST_F(LLVMGreaterThanTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "abC def", "abC def");
}

TEST_F(LLVMGreaterThanTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "abC def", "abc dEf");
}

TEST_F(LLVMGreaterThanTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "abC def", "abc dEf");
}

TEST_F(LLVMGreaterThanTest, FirstStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "abC def", "ghi Jkl");
}

TEST_F(LLVMGreaterThanTest, FirstStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "abC def", "ghi Jkl");
}

TEST_F(LLVMGreaterThanTest, SecondStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "ghi Jkl", "abC def");
}

TEST_F(LLVMGreaterThanTest, SecondStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "ghi Jkl", "abC def");
}

TEST_F(LLVMGreaterThanTest, StringsLengthComparison)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "abC def", "hello world");
}

TEST_F(LLVMGreaterThanTest, StringsLengthComparison_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "abC def", "hello world");
}

TEST_F(LLVMGreaterThanTest, SpaceGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, " ", "");
}

TEST_F(LLVMGreaterThanTest, SpaceGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, " ", "");
}

TEST_F(LLVMGreaterThanTest, SpaceGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, " ", "0");
}

TEST_F(LLVMGreaterThanTest, SpaceGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, " ", "0");
}

TEST_F(LLVMGreaterThanTest, SpaceGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, " ", 0);
}

TEST_F(LLVMGreaterThanTest, SpaceGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, " ", 0);
}

TEST_F(LLVMGreaterThanTest, ZeroNumberGreaterThanSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, " ");
}

TEST_F(LLVMGreaterThanTest, ZeroNumberGreaterThanSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, " ");
}

TEST_F(LLVMGreaterThanTest, EmptyGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "", "0");
}

TEST_F(LLVMGreaterThanTest, EmptyGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "", "0");
}

TEST_F(LLVMGreaterThanTest, EmptyGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "", 0);
}

TEST_F(LLVMGreaterThanTest, EmptyGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "", 0);
}

TEST_F(LLVMGreaterThanTest, ZeroNumberGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, "");
}

TEST_F(LLVMGreaterThanTest, ZeroNumberGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, "");
}

TEST_F(LLVMGreaterThanTest, ZeroStringGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "0", 0);
}

TEST_F(LLVMGreaterThanTest, ZeroStringGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "0", 0);
}

TEST_F(LLVMGreaterThanTest, ZeroNumberGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, "0");
}

TEST_F(LLVMGreaterThanTest, ZeroNumberGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, "0");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanExactString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5.25, "5.25");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5.25, "5.25");
}

TEST_F(LLVMGreaterThanTest, ExactStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "5.25", 5.25);
}

TEST_F(LLVMGreaterThanTest, ExactStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "5.25", 5.25);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5.25, " 5.25");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5.25, " 5.25");
}

TEST_F(LLVMGreaterThanTest, StringWithLeadingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, " 5.25", 5.25);
}

TEST_F(LLVMGreaterThanTest, StringWithLeadingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, " 5.25", 5.25);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5.25, "5.25 ");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5.25, "5.25 ");
}

TEST_F(LLVMGreaterThanTest, StringWithTrailingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "5.25 ", 5.25);
}

TEST_F(LLVMGreaterThanTest, StringWithTrailingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "5.25 ", 5.25);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5.25, " 5.25 ");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5.25, " 5.25 ");
}

TEST_F(LLVMGreaterThanTest, StringWithSurroundingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, " 5.25 ", 5.25);
}

TEST_F(LLVMGreaterThanTest, StringWithSurroundingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, " 5.25 ", 5.25);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5.25, "5.26");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5.25, "5.26");
}

TEST_F(LLVMGreaterThanTest, DifferentStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "5.26", 5.25);
}

TEST_F(LLVMGreaterThanTest, DifferentStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "5.26", 5.25);
}

TEST_F(LLVMGreaterThanTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "5.25", "5.26");
}

TEST_F(LLVMGreaterThanTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "5.25", "5.26");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5, "5  ");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5, "5  ");
}

TEST_F(LLVMGreaterThanTest, StringWithTrailingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "5  ", 5);
}

TEST_F(LLVMGreaterThanTest, StringWithTrailingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "5  ", 5);
}

TEST_F(LLVMGreaterThanTest, ZeroGreaterThanOneString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, "1");
}

TEST_F(LLVMGreaterThanTest, ZeroGreaterThanOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, "1");
}

TEST_F(LLVMGreaterThanTest, OneStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "1", 0);
}

TEST_F(LLVMGreaterThanTest, OneStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "1", 0);
}

TEST_F(LLVMGreaterThanTest, ZeroGreaterThanTestString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, "test");
}

TEST_F(LLVMGreaterThanTest, ZeroGreaterThanTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, "test");
}

TEST_F(LLVMGreaterThanTest, TestStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "test", 0);
}

TEST_F(LLVMGreaterThanTest, TestStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "test", 0);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanAbcString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 55, "abc");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanAbcString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 55, "abc");
}

TEST_F(LLVMGreaterThanTest, AbcStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "abc", 55);
}

TEST_F(LLVMGreaterThanTest, AbcStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "abc", 55);
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "NaN");
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5, "Infinity");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5, "Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", 5);
}

TEST_F(LLVMGreaterThanTest, InfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", 5);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5, "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5, "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", 5);
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", 5);
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 5, "NaN");
}

TEST_F(LLVMGreaterThanTest, NumberGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 5, "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", 5);
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", 5);
}

TEST_F(LLVMGreaterThanTest, ZeroGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, "NaN");
}

TEST_F(LLVMGreaterThanTest, ZeroGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", 0);
}

TEST_F(LLVMGreaterThanTest, NaNGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", 0);
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "true");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "true");
}

TEST_F(LLVMGreaterThanTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "true", true);
}

TEST_F(LLVMGreaterThanTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "true", true);
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, "false");
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, "false");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "false", false);
}

TEST_F(LLVMGreaterThanTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "false", false);
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, "true");
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, "true");
}

TEST_F(LLVMGreaterThanTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "true", false);
}

TEST_F(LLVMGreaterThanTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "true", false);
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "false");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "false");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "false", true);
}

TEST_F(LLVMGreaterThanTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "false", true);
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "TRUE");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "TRUE");
}

TEST_F(LLVMGreaterThanTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "TRUE", true);
}

TEST_F(LLVMGreaterThanTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "TRUE", true);
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, "FALSE");
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, "FALSE");
}

TEST_F(LLVMGreaterThanTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "FALSE", false);
}

TEST_F(LLVMGreaterThanTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "FALSE", false);
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "00001");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "00001");
}

TEST_F(LLVMGreaterThanTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "00001", true);
}

TEST_F(LLVMGreaterThanTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "00001", true);
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "00000");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "00000");
}

TEST_F(LLVMGreaterThanTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "00000", true);
}

TEST_F(LLVMGreaterThanTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "00000", true);
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, "00000");
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, "00000");
}

TEST_F(LLVMGreaterThanTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "00000", false);
}

TEST_F(LLVMGreaterThanTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "00000", false);
}

TEST_F(LLVMGreaterThanTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "true", 1);
}

TEST_F(LLVMGreaterThanTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "true", 1);
}

TEST_F(LLVMGreaterThanTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 1, "true");
}

TEST_F(LLVMGreaterThanTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 1, "true");
}

TEST_F(LLVMGreaterThanTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "true", 0);
}

TEST_F(LLVMGreaterThanTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "true", 0);
}

TEST_F(LLVMGreaterThanTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, "true");
}

TEST_F(LLVMGreaterThanTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, "true");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "false", 0);
}

TEST_F(LLVMGreaterThanTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "false", 0);
}

TEST_F(LLVMGreaterThanTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 0, "false");
}

TEST_F(LLVMGreaterThanTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 0, "false");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "false", 1);
}

TEST_F(LLVMGreaterThanTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "false", 1);
}

TEST_F(LLVMGreaterThanTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, 1, "false");
}

TEST_F(LLVMGreaterThanTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, 1, "false");
}

TEST_F(LLVMGreaterThanTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "true", "TRUE");
}

TEST_F(LLVMGreaterThanTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "true", "TRUE");
}

TEST_F(LLVMGreaterThanTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "true", "FALSE");
}

TEST_F(LLVMGreaterThanTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "true", "FALSE");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "false", "FALSE");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "false", "FALSE");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "false", "TRUE");
}

TEST_F(LLVMGreaterThanTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "false", "TRUE");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "Infinity");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", true);
}

TEST_F(LLVMGreaterThanTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", true);
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "-Infinity");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", true);
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", true);
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, true, "NaN");
}

TEST_F(LLVMGreaterThanTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, true, "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", true);
}

TEST_F(LLVMGreaterThanTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", true);
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, "Infinity");
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, "Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", false);
}

TEST_F(LLVMGreaterThanTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", false);
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, "-Infinity");
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", false);
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", false);
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, false, "NaN");
}

TEST_F(LLVMGreaterThanTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, false, "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", false);
}

TEST_F(LLVMGreaterThanTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", false);
}

TEST_F(LLVMGreaterThanTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-infinity", "Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-infinity", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-infinity", "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "Infinity");
}

TEST_F(LLVMGreaterThanTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "NaN");
}

TEST_F(LLVMGreaterThanTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "NaN");
}

TEST_F(LLVMGreaterThanTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "nan", "Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "nan", "Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "nan", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "nan", "-Infinity");
}

TEST_F(LLVMGreaterThanTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "nan", "NaN");
}

TEST_F(LLVMGreaterThanTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "nan", "NaN");
}

TEST_F(LLVMGreaterThanTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "abc");
}

TEST_F(LLVMGreaterThanTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "abc");
}

TEST_F(LLVMGreaterThanTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", " ");
}

TEST_F(LLVMGreaterThanTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", " ");
}

TEST_F(LLVMGreaterThanTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "");
}

TEST_F(LLVMGreaterThanTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "");
}

TEST_F(LLVMGreaterThanTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "Infinity", "0");
}

TEST_F(LLVMGreaterThanTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "Infinity", "0");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "abc");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "abc");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", " ");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", " ");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "-Infinity", "0");
}

TEST_F(LLVMGreaterThanTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "-Infinity", "0");
}

TEST_F(LLVMGreaterThanTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "abc");
}

TEST_F(LLVMGreaterThanTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "abc");
}

TEST_F(LLVMGreaterThanTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", " ");
}

TEST_F(LLVMGreaterThanTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", " ");
}

TEST_F(LLVMGreaterThanTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "");
}

TEST_F(LLVMGreaterThanTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "");
}

TEST_F(LLVMGreaterThanTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, false, "NaN", "0");
}

TEST_F(LLVMGreaterThanTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpGT, true, "NaN", "0");
}
