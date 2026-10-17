#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class GreaterThanTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(GreaterThanTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 10, 10);
}

TEST_F(GreaterThanTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 10, 10);
}

TEST_F(GreaterThanTest, FirstGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 10, 8);
}

TEST_F(GreaterThanTest, FirstGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 10, 8);
}

TEST_F(GreaterThanTest, SecondGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 8, 10);
}

TEST_F(GreaterThanTest, SecondGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 8, 10);
}

TEST_F(GreaterThanTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, -4.25, -4.25);
}

TEST_F(GreaterThanTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, -4.25, -4.25);
}

TEST_F(GreaterThanTest, FirstGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, -4.25, 5.312);
}

TEST_F(GreaterThanTest, FirstGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, -4.25, 5.312);
}

TEST_F(GreaterThanTest, SecondGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5.312, -4.25);
}

TEST_F(GreaterThanTest, SecondGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5.312, -4.25);
}

TEST_F(GreaterThanTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, true);
}

TEST_F(GreaterThanTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, true);
}

TEST_F(GreaterThanTest, TrueGreaterThanFalse)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, false);
}

TEST_F(GreaterThanTest, TrueGreaterThanFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, false);
}

TEST_F(GreaterThanTest, FalseGreaterThanTrue)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, true);
}

TEST_F(GreaterThanTest, FalseGreaterThanTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, true);
}

TEST_F(GreaterThanTest, NumberGreaterThanTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 1, true);
}

TEST_F(GreaterThanTest, NumberGreaterThanTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 1, true);
}

TEST_F(GreaterThanTest, NumberGreaterThanFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 1, false);
}

TEST_F(GreaterThanTest, NumberGreaterThanFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 1, false);
}

TEST_F(GreaterThanTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "abC def", "abC def");
}

TEST_F(GreaterThanTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "abC def", "abC def");
}

TEST_F(GreaterThanTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "abC def", "abc dEf");
}

TEST_F(GreaterThanTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "abC def", "abc dEf");
}

TEST_F(GreaterThanTest, FirstStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "abC def", "ghi Jkl");
}

TEST_F(GreaterThanTest, FirstStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "abC def", "ghi Jkl");
}

TEST_F(GreaterThanTest, SecondStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "ghi Jkl", "abC def");
}

TEST_F(GreaterThanTest, SecondStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "ghi Jkl", "abC def");
}

TEST_F(GreaterThanTest, StringsLengthComparison)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "abC def", "hello world");
}

TEST_F(GreaterThanTest, StringsLengthComparison_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "abC def", "hello world");
}

TEST_F(GreaterThanTest, SpaceGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, " ", "");
}

TEST_F(GreaterThanTest, SpaceGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, " ", "");
}

TEST_F(GreaterThanTest, SpaceGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, " ", "0");
}

TEST_F(GreaterThanTest, SpaceGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, " ", "0");
}

TEST_F(GreaterThanTest, SpaceGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, " ", 0);
}

TEST_F(GreaterThanTest, SpaceGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, " ", 0);
}

TEST_F(GreaterThanTest, ZeroNumberGreaterThanSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, " ");
}

TEST_F(GreaterThanTest, ZeroNumberGreaterThanSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, " ");
}

TEST_F(GreaterThanTest, EmptyGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "", "0");
}

TEST_F(GreaterThanTest, EmptyGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "", "0");
}

TEST_F(GreaterThanTest, EmptyGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "", 0);
}

TEST_F(GreaterThanTest, EmptyGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "", 0);
}

TEST_F(GreaterThanTest, ZeroNumberGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, "");
}

TEST_F(GreaterThanTest, ZeroNumberGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, "");
}

TEST_F(GreaterThanTest, ZeroStringGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "0", 0);
}

TEST_F(GreaterThanTest, ZeroStringGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "0", 0);
}

TEST_F(GreaterThanTest, ZeroNumberGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, "0");
}

TEST_F(GreaterThanTest, ZeroNumberGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, "0");
}

TEST_F(GreaterThanTest, NumberGreaterThanExactString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5.25, "5.25");
}

TEST_F(GreaterThanTest, NumberGreaterThanExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5.25, "5.25");
}

TEST_F(GreaterThanTest, ExactStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "5.25", 5.25);
}

TEST_F(GreaterThanTest, ExactStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "5.25", 5.25);
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5.25, " 5.25");
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5.25, " 5.25");
}

TEST_F(GreaterThanTest, StringWithLeadingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, " 5.25", 5.25);
}

TEST_F(GreaterThanTest, StringWithLeadingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, " 5.25", 5.25);
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5.25, "5.25 ");
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5.25, "5.25 ");
}

TEST_F(GreaterThanTest, StringWithTrailingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "5.25 ", 5.25);
}

TEST_F(GreaterThanTest, StringWithTrailingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "5.25 ", 5.25);
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5.25, " 5.25 ");
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5.25, " 5.25 ");
}

TEST_F(GreaterThanTest, StringWithSurroundingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, " 5.25 ", 5.25);
}

TEST_F(GreaterThanTest, StringWithSurroundingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, " 5.25 ", 5.25);
}

TEST_F(GreaterThanTest, NumberGreaterThanDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5.25, "5.26");
}

TEST_F(GreaterThanTest, NumberGreaterThanDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5.25, "5.26");
}

TEST_F(GreaterThanTest, DifferentStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "5.26", 5.25);
}

TEST_F(GreaterThanTest, DifferentStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "5.26", 5.25);
}

TEST_F(GreaterThanTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "5.25", "5.26");
}

TEST_F(GreaterThanTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "5.25", "5.26");
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5, "5  ");
}

TEST_F(GreaterThanTest, NumberGreaterThanStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5, "5  ");
}

TEST_F(GreaterThanTest, StringWithTrailingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "5  ", 5);
}

TEST_F(GreaterThanTest, StringWithTrailingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "5  ", 5);
}

TEST_F(GreaterThanTest, ZeroGreaterThanOneString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, "1");
}

TEST_F(GreaterThanTest, ZeroGreaterThanOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, "1");
}

TEST_F(GreaterThanTest, OneStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "1", 0);
}

TEST_F(GreaterThanTest, OneStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "1", 0);
}

TEST_F(GreaterThanTest, ZeroGreaterThanTestString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, "test");
}

TEST_F(GreaterThanTest, ZeroGreaterThanTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, "test");
}

TEST_F(GreaterThanTest, TestStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "test", 0);
}

TEST_F(GreaterThanTest, TestStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "test", 0);
}

TEST_F(GreaterThanTest, NumberGreaterThanAbcString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 55, "abc");
}

TEST_F(GreaterThanTest, NumberGreaterThanAbcString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 55, "abc");
}

TEST_F(GreaterThanTest, AbcStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "abc", 55);
}

TEST_F(GreaterThanTest, AbcStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "abc", 55);
}

TEST_F(GreaterThanTest, InfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "Infinity");
}

TEST_F(GreaterThanTest, InfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, NaNGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "NaN");
}

TEST_F(GreaterThanTest, NaNGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "NaN");
}

TEST_F(GreaterThanTest, InfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, InfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "Infinity");
}

TEST_F(GreaterThanTest, InfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "NaN");
}

TEST_F(GreaterThanTest, InfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "NaN");
}

TEST_F(GreaterThanTest, NaNGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "Infinity");
}

TEST_F(GreaterThanTest, NaNGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "NaN");
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "NaN");
}

TEST_F(GreaterThanTest, NaNGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "-Infinity");
}

TEST_F(GreaterThanTest, NaNGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "-Infinity");
}

TEST_F(GreaterThanTest, NumberGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5, "Infinity");
}

TEST_F(GreaterThanTest, NumberGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5, "Infinity");
}

TEST_F(GreaterThanTest, InfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", 5);
}

TEST_F(GreaterThanTest, InfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", 5);
}

TEST_F(GreaterThanTest, NumberGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5, "-Infinity");
}

TEST_F(GreaterThanTest, NumberGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5, "-Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", 5);
}

TEST_F(GreaterThanTest, NegativeInfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", 5);
}

TEST_F(GreaterThanTest, NumberGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 5, "NaN");
}

TEST_F(GreaterThanTest, NumberGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 5, "NaN");
}

TEST_F(GreaterThanTest, NaNGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", 5);
}

TEST_F(GreaterThanTest, NaNGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", 5);
}

TEST_F(GreaterThanTest, ZeroGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, "NaN");
}

TEST_F(GreaterThanTest, ZeroGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, "NaN");
}

TEST_F(GreaterThanTest, NaNGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", 0);
}

TEST_F(GreaterThanTest, NaNGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", 0);
}

TEST_F(GreaterThanTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "true");
}

TEST_F(GreaterThanTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "true");
}

TEST_F(GreaterThanTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "true", true);
}

TEST_F(GreaterThanTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "true", true);
}

TEST_F(GreaterThanTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, "false");
}

TEST_F(GreaterThanTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, "false");
}

TEST_F(GreaterThanTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "false", false);
}

TEST_F(GreaterThanTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "false", false);
}

TEST_F(GreaterThanTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, "true");
}

TEST_F(GreaterThanTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, "true");
}

TEST_F(GreaterThanTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "true", false);
}

TEST_F(GreaterThanTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "true", false);
}

TEST_F(GreaterThanTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "false");
}

TEST_F(GreaterThanTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "false");
}

TEST_F(GreaterThanTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "false", true);
}

TEST_F(GreaterThanTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "false", true);
}

TEST_F(GreaterThanTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "TRUE");
}

TEST_F(GreaterThanTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "TRUE");
}

TEST_F(GreaterThanTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "TRUE", true);
}

TEST_F(GreaterThanTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "TRUE", true);
}

TEST_F(GreaterThanTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, "FALSE");
}

TEST_F(GreaterThanTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, "FALSE");
}

TEST_F(GreaterThanTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "FALSE", false);
}

TEST_F(GreaterThanTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "FALSE", false);
}

TEST_F(GreaterThanTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "00001");
}

TEST_F(GreaterThanTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "00001");
}

TEST_F(GreaterThanTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "00001", true);
}

TEST_F(GreaterThanTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "00001", true);
}

TEST_F(GreaterThanTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "00000");
}

TEST_F(GreaterThanTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "00000");
}

TEST_F(GreaterThanTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "00000", true);
}

TEST_F(GreaterThanTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "00000", true);
}

TEST_F(GreaterThanTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, "00000");
}

TEST_F(GreaterThanTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, "00000");
}

TEST_F(GreaterThanTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "00000", false);
}

TEST_F(GreaterThanTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "00000", false);
}

TEST_F(GreaterThanTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "true", 1);
}

TEST_F(GreaterThanTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "true", 1);
}

TEST_F(GreaterThanTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 1, "true");
}

TEST_F(GreaterThanTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 1, "true");
}

TEST_F(GreaterThanTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "true", 0);
}

TEST_F(GreaterThanTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "true", 0);
}

TEST_F(GreaterThanTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, "true");
}

TEST_F(GreaterThanTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, "true");
}

TEST_F(GreaterThanTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "false", 0);
}

TEST_F(GreaterThanTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "false", 0);
}

TEST_F(GreaterThanTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 0, "false");
}

TEST_F(GreaterThanTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 0, "false");
}

TEST_F(GreaterThanTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "false", 1);
}

TEST_F(GreaterThanTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "false", 1);
}

TEST_F(GreaterThanTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, 1, "false");
}

TEST_F(GreaterThanTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, 1, "false");
}

TEST_F(GreaterThanTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "true", "TRUE");
}

TEST_F(GreaterThanTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "true", "TRUE");
}

TEST_F(GreaterThanTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "true", "FALSE");
}

TEST_F(GreaterThanTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "true", "FALSE");
}

TEST_F(GreaterThanTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "false", "FALSE");
}

TEST_F(GreaterThanTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "false", "FALSE");
}

TEST_F(GreaterThanTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "false", "TRUE");
}

TEST_F(GreaterThanTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "false", "TRUE");
}

TEST_F(GreaterThanTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "Infinity");
}

TEST_F(GreaterThanTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "Infinity");
}

TEST_F(GreaterThanTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", true);
}

TEST_F(GreaterThanTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", true);
}

TEST_F(GreaterThanTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "-Infinity");
}

TEST_F(GreaterThanTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "-Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", true);
}

TEST_F(GreaterThanTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", true);
}

TEST_F(GreaterThanTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, true, "NaN");
}

TEST_F(GreaterThanTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, true, "NaN");
}

TEST_F(GreaterThanTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", true);
}

TEST_F(GreaterThanTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", true);
}

TEST_F(GreaterThanTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, "Infinity");
}

TEST_F(GreaterThanTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, "Infinity");
}

TEST_F(GreaterThanTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", false);
}

TEST_F(GreaterThanTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", false);
}

TEST_F(GreaterThanTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, "-Infinity");
}

TEST_F(GreaterThanTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, "-Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", false);
}

TEST_F(GreaterThanTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", false);
}

TEST_F(GreaterThanTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, false, "NaN");
}

TEST_F(GreaterThanTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, false, "NaN");
}

TEST_F(GreaterThanTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", false);
}

TEST_F(GreaterThanTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", false);
}

TEST_F(GreaterThanTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "Infinity");
}

TEST_F(GreaterThanTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "Infinity");
}

TEST_F(GreaterThanTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "NaN");
}

TEST_F(GreaterThanTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "NaN");
}

TEST_F(GreaterThanTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "infinity", "Infinity");
}

TEST_F(GreaterThanTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "infinity", "Infinity");
}

TEST_F(GreaterThanTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "infinity", "-Infinity");
}

TEST_F(GreaterThanTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "infinity", "-Infinity");
}

TEST_F(GreaterThanTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "infinity", "NaN");
}

TEST_F(GreaterThanTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "infinity", "NaN");
}

TEST_F(GreaterThanTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "-Infinity");
}

TEST_F(GreaterThanTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "NaN");
}

TEST_F(GreaterThanTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "NaN");
}

TEST_F(GreaterThanTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-infinity", "Infinity");
}

TEST_F(GreaterThanTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-infinity", "Infinity");
}

TEST_F(GreaterThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-infinity", "-Infinity");
}

TEST_F(GreaterThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-infinity", "-Infinity");
}

TEST_F(GreaterThanTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-infinity", "NaN");
}

TEST_F(GreaterThanTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-infinity", "NaN");
}

TEST_F(GreaterThanTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "Infinity");
}

TEST_F(GreaterThanTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "Infinity");
}

TEST_F(GreaterThanTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "-Infinity");
}

TEST_F(GreaterThanTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "-Infinity");
}

TEST_F(GreaterThanTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "NaN");
}

TEST_F(GreaterThanTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "NaN");
}

TEST_F(GreaterThanTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "nan", "Infinity");
}

TEST_F(GreaterThanTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "nan", "Infinity");
}

TEST_F(GreaterThanTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "nan", "-Infinity");
}

TEST_F(GreaterThanTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "nan", "-Infinity");
}

TEST_F(GreaterThanTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "nan", "NaN");
}

TEST_F(GreaterThanTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "nan", "NaN");
}

TEST_F(GreaterThanTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "abc");
}

TEST_F(GreaterThanTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "abc");
}

TEST_F(GreaterThanTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", " ");
}

TEST_F(GreaterThanTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", " ");
}

TEST_F(GreaterThanTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "");
}

TEST_F(GreaterThanTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "");
}

TEST_F(GreaterThanTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "Infinity", "0");
}

TEST_F(GreaterThanTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "Infinity", "0");
}

TEST_F(GreaterThanTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "abc");
}

TEST_F(GreaterThanTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "abc");
}

TEST_F(GreaterThanTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", " ");
}

TEST_F(GreaterThanTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", " ");
}

TEST_F(GreaterThanTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "");
}

TEST_F(GreaterThanTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "");
}

TEST_F(GreaterThanTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "-Infinity", "0");
}

TEST_F(GreaterThanTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "-Infinity", "0");
}

TEST_F(GreaterThanTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "abc");
}

TEST_F(GreaterThanTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "abc");
}

TEST_F(GreaterThanTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", " ");
}

TEST_F(GreaterThanTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", " ");
}

TEST_F(GreaterThanTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "");
}

TEST_F(GreaterThanTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "");
}

TEST_F(GreaterThanTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, false, "NaN", "0");
}

TEST_F(GreaterThanTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpGT, true, "NaN", "0");
}
