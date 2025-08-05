#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMLessThanTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMLessThanTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 10, 10);
}

TEST_F(LLVMLessThanTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 10, 10);
}

TEST_F(LLVMLessThanTest, FirstGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 10, 8);
}

TEST_F(LLVMLessThanTest, FirstGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 10, 8);
}

TEST_F(LLVMLessThanTest, SecondGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 8, 10);
}

TEST_F(LLVMLessThanTest, SecondGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 8, 10);
}

TEST_F(LLVMLessThanTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, -4.25, -4.25);
}

TEST_F(LLVMLessThanTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, -4.25, -4.25);
}

TEST_F(LLVMLessThanTest, FirstGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, -4.25, 5.312);
}

TEST_F(LLVMLessThanTest, FirstGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, -4.25, 5.312);
}

TEST_F(LLVMLessThanTest, SecondGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5.312, -4.25);
}

TEST_F(LLVMLessThanTest, SecondGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5.312, -4.25);
}

TEST_F(LLVMLessThanTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, true);
}

TEST_F(LLVMLessThanTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, true);
}

TEST_F(LLVMLessThanTest, TrueGreaterThanFalse)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, false);
}

TEST_F(LLVMLessThanTest, TrueGreaterThanFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, false);
}

TEST_F(LLVMLessThanTest, FalseGreaterThanTrue)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, true);
}

TEST_F(LLVMLessThanTest, FalseGreaterThanTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, true);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 1, true);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 1, true);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 1, false);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 1, false);
}

TEST_F(LLVMLessThanTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "abC def", "abC def");
}

TEST_F(LLVMLessThanTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "abC def", "abC def");
}

TEST_F(LLVMLessThanTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "abC def", "abc dEf");
}

TEST_F(LLVMLessThanTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "abC def", "abc dEf");
}

TEST_F(LLVMLessThanTest, FirstStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "abC def", "ghi Jkl");
}

TEST_F(LLVMLessThanTest, FirstStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "abC def", "ghi Jkl");
}

TEST_F(LLVMLessThanTest, SecondStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "ghi Jkl", "abC def");
}

TEST_F(LLVMLessThanTest, SecondStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "ghi Jkl", "abC def");
}

TEST_F(LLVMLessThanTest, StringsLengthComparison)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "abC def", "hello world");
}

TEST_F(LLVMLessThanTest, StringsLengthComparison_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "abC def", "hello world");
}

TEST_F(LLVMLessThanTest, SpaceGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, " ", "");
}

TEST_F(LLVMLessThanTest, SpaceGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, " ", "");
}

TEST_F(LLVMLessThanTest, SpaceGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, " ", "0");
}

TEST_F(LLVMLessThanTest, SpaceGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, " ", "0");
}

TEST_F(LLVMLessThanTest, SpaceGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, " ", 0);
}

TEST_F(LLVMLessThanTest, SpaceGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, " ", 0);
}

TEST_F(LLVMLessThanTest, ZeroNumberGreaterThanSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, " ");
}

TEST_F(LLVMLessThanTest, ZeroNumberGreaterThanSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, " ");
}

TEST_F(LLVMLessThanTest, EmptyGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "", "0");
}

TEST_F(LLVMLessThanTest, EmptyGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "", "0");
}

TEST_F(LLVMLessThanTest, EmptyGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "", 0);
}

TEST_F(LLVMLessThanTest, EmptyGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "", 0);
}

TEST_F(LLVMLessThanTest, ZeroNumberGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, "");
}

TEST_F(LLVMLessThanTest, ZeroNumberGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, "");
}

TEST_F(LLVMLessThanTest, ZeroStringGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "0", 0);
}

TEST_F(LLVMLessThanTest, ZeroStringGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "0", 0);
}

TEST_F(LLVMLessThanTest, ZeroNumberGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, "0");
}

TEST_F(LLVMLessThanTest, ZeroNumberGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, "0");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanExactString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5.25, "5.25");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5.25, "5.25");
}

TEST_F(LLVMLessThanTest, ExactStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "5.25", 5.25);
}

TEST_F(LLVMLessThanTest, ExactStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "5.25", 5.25);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5.25, " 5.25");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5.25, " 5.25");
}

TEST_F(LLVMLessThanTest, StringWithLeadingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, " 5.25", 5.25);
}

TEST_F(LLVMLessThanTest, StringWithLeadingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, " 5.25", 5.25);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5.25, "5.25 ");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5.25, "5.25 ");
}

TEST_F(LLVMLessThanTest, StringWithTrailingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "5.25 ", 5.25);
}

TEST_F(LLVMLessThanTest, StringWithTrailingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "5.25 ", 5.25);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5.25, " 5.25 ");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5.25, " 5.25 ");
}

TEST_F(LLVMLessThanTest, StringWithSurroundingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, " 5.25 ", 5.25);
}

TEST_F(LLVMLessThanTest, StringWithSurroundingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, " 5.25 ", 5.25);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5.25, "5.26");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5.25, "5.26");
}

TEST_F(LLVMLessThanTest, DifferentStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "5.26", 5.25);
}

TEST_F(LLVMLessThanTest, DifferentStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "5.26", 5.25);
}

TEST_F(LLVMLessThanTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "5.25", "5.26");
}

TEST_F(LLVMLessThanTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "5.25", "5.26");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5, "5  ");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5, "5  ");
}

TEST_F(LLVMLessThanTest, StringWithTrailingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "5  ", 5);
}

TEST_F(LLVMLessThanTest, StringWithTrailingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "5  ", 5);
}

TEST_F(LLVMLessThanTest, ZeroGreaterThanOneString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, "1");
}

TEST_F(LLVMLessThanTest, ZeroGreaterThanOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, "1");
}

TEST_F(LLVMLessThanTest, OneStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "1", 0);
}

TEST_F(LLVMLessThanTest, OneStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "1", 0);
}

TEST_F(LLVMLessThanTest, ZeroGreaterThanTestString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, "test");
}

TEST_F(LLVMLessThanTest, ZeroGreaterThanTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, "test");
}

TEST_F(LLVMLessThanTest, TestStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "test", 0);
}

TEST_F(LLVMLessThanTest, TestStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "test", 0);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanAbcString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 55, "abc");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanAbcString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 55, "abc");
}

TEST_F(LLVMLessThanTest, AbcStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "abc", 55);
}

TEST_F(LLVMLessThanTest, AbcStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "abc", 55);
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "NaN");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "NaN");
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "Infinity");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "-Infinity");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "-Infinity");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5, "Infinity");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5, "Infinity");
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", 5);
}

TEST_F(LLVMLessThanTest, InfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", 5);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5, "-Infinity");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5, "-Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", 5);
}

TEST_F(LLVMLessThanTest, NegativeInfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", 5);
}

TEST_F(LLVMLessThanTest, NumberGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 5, "NaN");
}

TEST_F(LLVMLessThanTest, NumberGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 5, "NaN");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", 5);
}

TEST_F(LLVMLessThanTest, NaNGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", 5);
}

TEST_F(LLVMLessThanTest, ZeroGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, "NaN");
}

TEST_F(LLVMLessThanTest, ZeroGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, "NaN");
}

TEST_F(LLVMLessThanTest, NaNGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", 0);
}

TEST_F(LLVMLessThanTest, NaNGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", 0);
}

TEST_F(LLVMLessThanTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "true");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "true");
}

TEST_F(LLVMLessThanTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "true", true);
}

TEST_F(LLVMLessThanTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "true", true);
}

TEST_F(LLVMLessThanTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, "false");
}

TEST_F(LLVMLessThanTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, "false");
}

TEST_F(LLVMLessThanTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "false", false);
}

TEST_F(LLVMLessThanTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "false", false);
}

TEST_F(LLVMLessThanTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, "true");
}

TEST_F(LLVMLessThanTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, "true");
}

TEST_F(LLVMLessThanTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "true", false);
}

TEST_F(LLVMLessThanTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "true", false);
}

TEST_F(LLVMLessThanTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "false");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "false");
}

TEST_F(LLVMLessThanTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "false", true);
}

TEST_F(LLVMLessThanTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "false", true);
}

TEST_F(LLVMLessThanTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "TRUE");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "TRUE");
}

TEST_F(LLVMLessThanTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "TRUE", true);
}

TEST_F(LLVMLessThanTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "TRUE", true);
}

TEST_F(LLVMLessThanTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, "FALSE");
}

TEST_F(LLVMLessThanTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, "FALSE");
}

TEST_F(LLVMLessThanTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "FALSE", false);
}

TEST_F(LLVMLessThanTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "FALSE", false);
}

TEST_F(LLVMLessThanTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "00001");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "00001");
}

TEST_F(LLVMLessThanTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "00001", true);
}

TEST_F(LLVMLessThanTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "00001", true);
}

TEST_F(LLVMLessThanTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "00000");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "00000");
}

TEST_F(LLVMLessThanTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "00000", true);
}

TEST_F(LLVMLessThanTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "00000", true);
}

TEST_F(LLVMLessThanTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, "00000");
}

TEST_F(LLVMLessThanTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, "00000");
}

TEST_F(LLVMLessThanTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "00000", false);
}

TEST_F(LLVMLessThanTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "00000", false);
}

TEST_F(LLVMLessThanTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "true", 1);
}

TEST_F(LLVMLessThanTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "true", 1);
}

TEST_F(LLVMLessThanTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 1, "true");
}

TEST_F(LLVMLessThanTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 1, "true");
}

TEST_F(LLVMLessThanTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "true", 0);
}

TEST_F(LLVMLessThanTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "true", 0);
}

TEST_F(LLVMLessThanTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, "true");
}

TEST_F(LLVMLessThanTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, "true");
}

TEST_F(LLVMLessThanTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "false", 0);
}

TEST_F(LLVMLessThanTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "false", 0);
}

TEST_F(LLVMLessThanTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 0, "false");
}

TEST_F(LLVMLessThanTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 0, "false");
}

TEST_F(LLVMLessThanTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "false", 1);
}

TEST_F(LLVMLessThanTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "false", 1);
}

TEST_F(LLVMLessThanTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, 1, "false");
}

TEST_F(LLVMLessThanTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, 1, "false");
}

TEST_F(LLVMLessThanTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "true", "TRUE");
}

TEST_F(LLVMLessThanTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "true", "TRUE");
}

TEST_F(LLVMLessThanTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "true", "FALSE");
}

TEST_F(LLVMLessThanTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "true", "FALSE");
}

TEST_F(LLVMLessThanTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "false", "FALSE");
}

TEST_F(LLVMLessThanTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "false", "FALSE");
}

TEST_F(LLVMLessThanTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "false", "TRUE");
}

TEST_F(LLVMLessThanTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "false", "TRUE");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "Infinity");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "Infinity");
}

TEST_F(LLVMLessThanTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", true);
}

TEST_F(LLVMLessThanTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", true);
}

TEST_F(LLVMLessThanTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "-Infinity");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "-Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", true);
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", true);
}

TEST_F(LLVMLessThanTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, true, "NaN");
}

TEST_F(LLVMLessThanTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, true, "NaN");
}

TEST_F(LLVMLessThanTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", true);
}

TEST_F(LLVMLessThanTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", true);
}

TEST_F(LLVMLessThanTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, "Infinity");
}

TEST_F(LLVMLessThanTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, "Infinity");
}

TEST_F(LLVMLessThanTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", false);
}

TEST_F(LLVMLessThanTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", false);
}

TEST_F(LLVMLessThanTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, "-Infinity");
}

TEST_F(LLVMLessThanTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, "-Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", false);
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", false);
}

TEST_F(LLVMLessThanTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, false, "NaN");
}

TEST_F(LLVMLessThanTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, false, "NaN");
}

TEST_F(LLVMLessThanTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", false);
}

TEST_F(LLVMLessThanTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", false);
}

TEST_F(LLVMLessThanTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "infinity", "NaN");
}

TEST_F(LLVMLessThanTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "infinity", "NaN");
}

TEST_F(LLVMLessThanTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "NaN");
}

TEST_F(LLVMLessThanTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-infinity", "Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-infinity", "-Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-infinity", "NaN");
}

TEST_F(LLVMLessThanTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-infinity", "NaN");
}

TEST_F(LLVMLessThanTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "Infinity");
}

TEST_F(LLVMLessThanTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "Infinity");
}

TEST_F(LLVMLessThanTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "-Infinity");
}

TEST_F(LLVMLessThanTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "-Infinity");
}

TEST_F(LLVMLessThanTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "NaN");
}

TEST_F(LLVMLessThanTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "NaN");
}

TEST_F(LLVMLessThanTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "nan", "Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "nan", "Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "nan", "-Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "nan", "-Infinity");
}

TEST_F(LLVMLessThanTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "nan", "NaN");
}

TEST_F(LLVMLessThanTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "nan", "NaN");
}

TEST_F(LLVMLessThanTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "abc");
}

TEST_F(LLVMLessThanTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "abc");
}

TEST_F(LLVMLessThanTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", " ");
}

TEST_F(LLVMLessThanTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", " ");
}

TEST_F(LLVMLessThanTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "");
}

TEST_F(LLVMLessThanTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "");
}

TEST_F(LLVMLessThanTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "Infinity", "0");
}

TEST_F(LLVMLessThanTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "Infinity", "0");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "abc");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "abc");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", " ");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", " ");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "-Infinity", "0");
}

TEST_F(LLVMLessThanTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "-Infinity", "0");
}

TEST_F(LLVMLessThanTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "abc");
}

TEST_F(LLVMLessThanTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "abc");
}

TEST_F(LLVMLessThanTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", " ");
}

TEST_F(LLVMLessThanTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", " ");
}

TEST_F(LLVMLessThanTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "");
}

TEST_F(LLVMLessThanTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "");
}

TEST_F(LLVMLessThanTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, false, "NaN", "0");
}

TEST_F(LLVMLessThanTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, LLVMTestUtils::OpType::CmpLT, true, "NaN", "0");
}
