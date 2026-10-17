#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class LessThanTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(LessThanTest, SameIntegers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 10, 10);
}

TEST_F(LessThanTest, SameIntegers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 10, 10);
}

TEST_F(LessThanTest, FirstGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 10, 8);
}

TEST_F(LessThanTest, FirstGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 10, 8);
}

TEST_F(LessThanTest, SecondGreaterInteger)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 8, 10);
}

TEST_F(LessThanTest, SecondGreaterInteger_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 8, 10);
}

TEST_F(LessThanTest, SameDecimals)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, -4.25, -4.25);
}

TEST_F(LessThanTest, SameDecimals_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, -4.25, -4.25);
}

TEST_F(LessThanTest, FirstGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, -4.25, 5.312);
}

TEST_F(LessThanTest, FirstGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, -4.25, 5.312);
}

TEST_F(LessThanTest, SecondGreaterDecimal)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5.312, -4.25);
}

TEST_F(LessThanTest, SecondGreaterDecimal_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5.312, -4.25);
}

TEST_F(LessThanTest, SameBooleans)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, true);
}

TEST_F(LessThanTest, SameBooleans_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, true);
}

TEST_F(LessThanTest, TrueGreaterThanFalse)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, false);
}

TEST_F(LessThanTest, TrueGreaterThanFalse_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, false);
}

TEST_F(LessThanTest, FalseGreaterThanTrue)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, true);
}

TEST_F(LessThanTest, FalseGreaterThanTrue_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, true);
}

TEST_F(LessThanTest, NumberGreaterThanTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 1, true);
}

TEST_F(LessThanTest, NumberGreaterThanTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 1, true);
}

TEST_F(LessThanTest, NumberGreaterThanFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 1, false);
}

TEST_F(LessThanTest, NumberGreaterThanFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 1, false);
}

TEST_F(LessThanTest, SameStrings)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "abC def", "abC def");
}

TEST_F(LessThanTest, SameStrings_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "abC def", "abC def");
}

TEST_F(LessThanTest, StringsCaseDifferent)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "abC def", "abc dEf");
}

TEST_F(LessThanTest, StringsCaseDifferent_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "abC def", "abc dEf");
}

TEST_F(LessThanTest, FirstStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "abC def", "ghi Jkl");
}

TEST_F(LessThanTest, FirstStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "abC def", "ghi Jkl");
}

TEST_F(LessThanTest, SecondStringGreater)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "ghi Jkl", "abC def");
}

TEST_F(LessThanTest, SecondStringGreater_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "ghi Jkl", "abC def");
}

TEST_F(LessThanTest, StringsLengthComparison)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "abC def", "hello world");
}

TEST_F(LessThanTest, StringsLengthComparison_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "abC def", "hello world");
}

TEST_F(LessThanTest, SpaceGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, " ", "");
}

TEST_F(LessThanTest, SpaceGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, " ", "");
}

TEST_F(LessThanTest, SpaceGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, " ", "0");
}

TEST_F(LessThanTest, SpaceGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, " ", "0");
}

TEST_F(LessThanTest, SpaceGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, " ", 0);
}

TEST_F(LessThanTest, SpaceGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, " ", 0);
}

TEST_F(LessThanTest, ZeroNumberGreaterThanSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, " ");
}

TEST_F(LessThanTest, ZeroNumberGreaterThanSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, " ");
}

TEST_F(LessThanTest, EmptyGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "", "0");
}

TEST_F(LessThanTest, EmptyGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "", "0");
}

TEST_F(LessThanTest, EmptyGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "", 0);
}

TEST_F(LessThanTest, EmptyGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "", 0);
}

TEST_F(LessThanTest, ZeroNumberGreaterThanEmpty)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, "");
}

TEST_F(LessThanTest, ZeroNumberGreaterThanEmpty_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, "");
}

TEST_F(LessThanTest, ZeroStringGreaterThanZeroNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "0", 0);
}

TEST_F(LessThanTest, ZeroStringGreaterThanZeroNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "0", 0);
}

TEST_F(LessThanTest, ZeroNumberGreaterThanZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, "0");
}

TEST_F(LessThanTest, ZeroNumberGreaterThanZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, "0");
}

TEST_F(LessThanTest, NumberGreaterThanExactString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5.25, "5.25");
}

TEST_F(LessThanTest, NumberGreaterThanExactString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5.25, "5.25");
}

TEST_F(LessThanTest, ExactStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "5.25", 5.25);
}

TEST_F(LessThanTest, ExactStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "5.25", 5.25);
}

TEST_F(LessThanTest, NumberGreaterThanStringWithLeadingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5.25, " 5.25");
}

TEST_F(LessThanTest, NumberGreaterThanStringWithLeadingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5.25, " 5.25");
}

TEST_F(LessThanTest, StringWithLeadingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, " 5.25", 5.25);
}

TEST_F(LessThanTest, StringWithLeadingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, " 5.25", 5.25);
}

TEST_F(LessThanTest, NumberGreaterThanStringWithTrailingSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5.25, "5.25 ");
}

TEST_F(LessThanTest, NumberGreaterThanStringWithTrailingSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5.25, "5.25 ");
}

TEST_F(LessThanTest, StringWithTrailingSpaceGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "5.25 ", 5.25);
}

TEST_F(LessThanTest, StringWithTrailingSpaceGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "5.25 ", 5.25);
}

TEST_F(LessThanTest, NumberGreaterThanStringWithSurroundingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5.25, " 5.25 ");
}

TEST_F(LessThanTest, NumberGreaterThanStringWithSurroundingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5.25, " 5.25 ");
}

TEST_F(LessThanTest, StringWithSurroundingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, " 5.25 ", 5.25);
}

TEST_F(LessThanTest, StringWithSurroundingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, " 5.25 ", 5.25);
}

TEST_F(LessThanTest, NumberGreaterThanDifferentString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5.25, "5.26");
}

TEST_F(LessThanTest, NumberGreaterThanDifferentString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5.25, "5.26");
}

TEST_F(LessThanTest, DifferentStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "5.26", 5.25);
}

TEST_F(LessThanTest, DifferentStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "5.26", 5.25);
}

TEST_F(LessThanTest, DifferentStringNumbers)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "5.25", "5.26");
}

TEST_F(LessThanTest, DifferentStringNumbers_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "5.25", "5.26");
}

TEST_F(LessThanTest, NumberGreaterThanStringWithTrailingSpaces)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5, "5  ");
}

TEST_F(LessThanTest, NumberGreaterThanStringWithTrailingSpaces_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5, "5  ");
}

TEST_F(LessThanTest, StringWithTrailingSpacesGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "5  ", 5);
}

TEST_F(LessThanTest, StringWithTrailingSpacesGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "5  ", 5);
}

TEST_F(LessThanTest, ZeroGreaterThanOneString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, "1");
}

TEST_F(LessThanTest, ZeroGreaterThanOneString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, "1");
}

TEST_F(LessThanTest, OneStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "1", 0);
}

TEST_F(LessThanTest, OneStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "1", 0);
}

TEST_F(LessThanTest, ZeroGreaterThanTestString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, "test");
}

TEST_F(LessThanTest, ZeroGreaterThanTestString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, "test");
}

TEST_F(LessThanTest, TestStringGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "test", 0);
}

TEST_F(LessThanTest, TestStringGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "test", 0);
}

TEST_F(LessThanTest, NumberGreaterThanAbcString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 55, "abc");
}

TEST_F(LessThanTest, NumberGreaterThanAbcString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 55, "abc");
}

TEST_F(LessThanTest, AbcStringGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "abc", 55);
}

TEST_F(LessThanTest, AbcStringGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "abc", 55);
}

TEST_F(LessThanTest, InfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "Infinity");
}

TEST_F(LessThanTest, InfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "Infinity");
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "-Infinity");
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "-Infinity");
}

TEST_F(LessThanTest, NaNGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "NaN");
}

TEST_F(LessThanTest, NaNGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "NaN");
}

TEST_F(LessThanTest, InfinityGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "-Infinity");
}

TEST_F(LessThanTest, InfinityGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "-Infinity");
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "Infinity");
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "Infinity");
}

TEST_F(LessThanTest, InfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "NaN");
}

TEST_F(LessThanTest, InfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "NaN");
}

TEST_F(LessThanTest, NaNGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "Infinity");
}

TEST_F(LessThanTest, NaNGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "Infinity");
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "NaN");
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "NaN");
}

TEST_F(LessThanTest, NaNGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "-Infinity");
}

TEST_F(LessThanTest, NaNGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "-Infinity");
}

TEST_F(LessThanTest, NumberGreaterThanInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5, "Infinity");
}

TEST_F(LessThanTest, NumberGreaterThanInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5, "Infinity");
}

TEST_F(LessThanTest, InfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", 5);
}

TEST_F(LessThanTest, InfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", 5);
}

TEST_F(LessThanTest, NumberGreaterThanNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5, "-Infinity");
}

TEST_F(LessThanTest, NumberGreaterThanNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5, "-Infinity");
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", 5);
}

TEST_F(LessThanTest, NegativeInfinityGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", 5);
}

TEST_F(LessThanTest, NumberGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 5, "NaN");
}

TEST_F(LessThanTest, NumberGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 5, "NaN");
}

TEST_F(LessThanTest, NaNGreaterThanNumber)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", 5);
}

TEST_F(LessThanTest, NaNGreaterThanNumber_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", 5);
}

TEST_F(LessThanTest, ZeroGreaterThanNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, "NaN");
}

TEST_F(LessThanTest, ZeroGreaterThanNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, "NaN");
}

TEST_F(LessThanTest, NaNGreaterThanZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", 0);
}

TEST_F(LessThanTest, NaNGreaterThanZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", 0);
}

TEST_F(LessThanTest, TrueBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "true");
}

TEST_F(LessThanTest, TrueBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "true");
}

TEST_F(LessThanTest, TrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "true", true);
}

TEST_F(LessThanTest, TrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "true", true);
}

TEST_F(LessThanTest, FalseBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, "false");
}

TEST_F(LessThanTest, FalseBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, "false");
}

TEST_F(LessThanTest, FalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "false", false);
}

TEST_F(LessThanTest, FalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "false", false);
}

TEST_F(LessThanTest, FalseBooleanWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, "true");
}

TEST_F(LessThanTest, FalseBooleanWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, "true");
}

TEST_F(LessThanTest, TrueStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "true", false);
}

TEST_F(LessThanTest, TrueStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "true", false);
}

TEST_F(LessThanTest, TrueBooleanWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "false");
}

TEST_F(LessThanTest, TrueBooleanWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "false");
}

TEST_F(LessThanTest, FalseStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "false", true);
}

TEST_F(LessThanTest, FalseStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "false", true);
}

TEST_F(LessThanTest, TrueBooleanWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "TRUE");
}

TEST_F(LessThanTest, TrueBooleanWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "TRUE");
}

TEST_F(LessThanTest, UppercaseTrueStringWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "TRUE", true);
}

TEST_F(LessThanTest, UppercaseTrueStringWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "TRUE", true);
}

TEST_F(LessThanTest, FalseBooleanWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, "FALSE");
}

TEST_F(LessThanTest, FalseBooleanWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, "FALSE");
}

TEST_F(LessThanTest, UppercaseFalseStringWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "FALSE", false);
}

TEST_F(LessThanTest, UppercaseFalseStringWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "FALSE", false);
}

TEST_F(LessThanTest, TrueBooleanWithZeroPaddedOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "00001");
}

TEST_F(LessThanTest, TrueBooleanWithZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "00001");
}

TEST_F(LessThanTest, ZeroPaddedOneWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "00001", true);
}

TEST_F(LessThanTest, ZeroPaddedOneWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "00001", true);
}

TEST_F(LessThanTest, TrueBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "00000");
}

TEST_F(LessThanTest, TrueBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "00000");
}

TEST_F(LessThanTest, ZeroPaddedZeroWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "00000", true);
}

TEST_F(LessThanTest, ZeroPaddedZeroWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "00000", true);
}

TEST_F(LessThanTest, FalseBooleanWithZeroPaddedZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, "00000");
}

TEST_F(LessThanTest, FalseBooleanWithZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, "00000");
}

TEST_F(LessThanTest, ZeroPaddedZeroWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "00000", false);
}

TEST_F(LessThanTest, ZeroPaddedZeroWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "00000", false);
}

TEST_F(LessThanTest, TrueStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "true", 1);
}

TEST_F(LessThanTest, TrueStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "true", 1);
}

TEST_F(LessThanTest, OneWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 1, "true");
}

TEST_F(LessThanTest, OneWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 1, "true");
}

TEST_F(LessThanTest, TrueStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "true", 0);
}

TEST_F(LessThanTest, TrueStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "true", 0);
}

TEST_F(LessThanTest, ZeroWithTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, "true");
}

TEST_F(LessThanTest, ZeroWithTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, "true");
}

TEST_F(LessThanTest, FalseStringWithZero)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "false", 0);
}

TEST_F(LessThanTest, FalseStringWithZero_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "false", 0);
}

TEST_F(LessThanTest, ZeroWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 0, "false");
}

TEST_F(LessThanTest, ZeroWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 0, "false");
}

TEST_F(LessThanTest, FalseStringWithOne)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "false", 1);
}

TEST_F(LessThanTest, FalseStringWithOne_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "false", 1);
}

TEST_F(LessThanTest, OneWithFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, 1, "false");
}

TEST_F(LessThanTest, OneWithFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, 1, "false");
}

TEST_F(LessThanTest, TrueStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "true", "TRUE");
}

TEST_F(LessThanTest, TrueStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "true", "TRUE");
}

TEST_F(LessThanTest, TrueStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "true", "FALSE");
}

TEST_F(LessThanTest, TrueStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "true", "FALSE");
}

TEST_F(LessThanTest, FalseStringWithUppercaseFalseString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "false", "FALSE");
}

TEST_F(LessThanTest, FalseStringWithUppercaseFalseString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "false", "FALSE");
}

TEST_F(LessThanTest, FalseStringWithUppercaseTrueString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "false", "TRUE");
}

TEST_F(LessThanTest, FalseStringWithUppercaseTrueString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "false", "TRUE");
}

TEST_F(LessThanTest, TrueBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "Infinity");
}

TEST_F(LessThanTest, TrueBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "Infinity");
}

TEST_F(LessThanTest, InfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", true);
}

TEST_F(LessThanTest, InfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", true);
}

TEST_F(LessThanTest, TrueBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "-Infinity");
}

TEST_F(LessThanTest, TrueBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "-Infinity");
}

TEST_F(LessThanTest, NegativeInfinityWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", true);
}

TEST_F(LessThanTest, NegativeInfinityWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", true);
}

TEST_F(LessThanTest, TrueBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, true, "NaN");
}

TEST_F(LessThanTest, TrueBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, true, "NaN");
}

TEST_F(LessThanTest, NaNWithTrueBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", true);
}

TEST_F(LessThanTest, NaNWithTrueBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", true);
}

TEST_F(LessThanTest, FalseBooleanWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, "Infinity");
}

TEST_F(LessThanTest, FalseBooleanWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, "Infinity");
}

TEST_F(LessThanTest, InfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", false);
}

TEST_F(LessThanTest, InfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", false);
}

TEST_F(LessThanTest, FalseBooleanWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, "-Infinity");
}

TEST_F(LessThanTest, FalseBooleanWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, "-Infinity");
}

TEST_F(LessThanTest, NegativeInfinityWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", false);
}

TEST_F(LessThanTest, NegativeInfinityWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", false);
}

TEST_F(LessThanTest, FalseBooleanWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, false, "NaN");
}

TEST_F(LessThanTest, FalseBooleanWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, false, "NaN");
}

TEST_F(LessThanTest, NaNWithFalseBoolean)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", false);
}

TEST_F(LessThanTest, NaNWithFalseBoolean_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", false);
}

TEST_F(LessThanTest, InfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "Infinity");
}

TEST_F(LessThanTest, InfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "Infinity");
}

TEST_F(LessThanTest, InfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "-Infinity");
}

TEST_F(LessThanTest, InfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "-Infinity");
}

TEST_F(LessThanTest, InfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "NaN");
}

TEST_F(LessThanTest, InfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "NaN");
}

TEST_F(LessThanTest, LowercaseInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "infinity", "Infinity");
}

TEST_F(LessThanTest, LowercaseInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "infinity", "Infinity");
}

TEST_F(LessThanTest, LowercaseInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "infinity", "-Infinity");
}

TEST_F(LessThanTest, LowercaseInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "infinity", "-Infinity");
}

TEST_F(LessThanTest, LowercaseInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "infinity", "NaN");
}

TEST_F(LessThanTest, LowercaseInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "infinity", "NaN");
}

TEST_F(LessThanTest, NegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "Infinity");
}

TEST_F(LessThanTest, NegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "Infinity");
}

TEST_F(LessThanTest, NegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "-Infinity");
}

TEST_F(LessThanTest, NegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "-Infinity");
}

TEST_F(LessThanTest, NegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "NaN");
}

TEST_F(LessThanTest, NegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "NaN");
}

TEST_F(LessThanTest, LowercaseNegativeInfinityStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-infinity", "Infinity");
}

TEST_F(LessThanTest, LowercaseNegativeInfinityStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-infinity", "Infinity");
}

TEST_F(LessThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-infinity", "-Infinity");
}

TEST_F(LessThanTest, LowercaseNegativeInfinityStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-infinity", "-Infinity");
}

TEST_F(LessThanTest, LowercaseNegativeInfinityStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-infinity", "NaN");
}

TEST_F(LessThanTest, LowercaseNegativeInfinityStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-infinity", "NaN");
}

TEST_F(LessThanTest, NaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "Infinity");
}

TEST_F(LessThanTest, NaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "Infinity");
}

TEST_F(LessThanTest, NaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "-Infinity");
}

TEST_F(LessThanTest, NaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "-Infinity");
}

TEST_F(LessThanTest, NaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "NaN");
}

TEST_F(LessThanTest, NaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "NaN");
}

TEST_F(LessThanTest, LowercaseNaNStringWithInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "nan", "Infinity");
}

TEST_F(LessThanTest, LowercaseNaNStringWithInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "nan", "Infinity");
}

TEST_F(LessThanTest, LowercaseNaNStringWithNegativeInfinity)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "nan", "-Infinity");
}

TEST_F(LessThanTest, LowercaseNaNStringWithNegativeInfinity_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "nan", "-Infinity");
}

TEST_F(LessThanTest, LowercaseNaNStringWithNaN)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "nan", "NaN");
}

TEST_F(LessThanTest, LowercaseNaNStringWithNaN_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "nan", "NaN");
}

TEST_F(LessThanTest, InfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "abc");
}

TEST_F(LessThanTest, InfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "abc");
}

TEST_F(LessThanTest, InfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", " ");
}

TEST_F(LessThanTest, InfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", " ");
}

TEST_F(LessThanTest, InfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "");
}

TEST_F(LessThanTest, InfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "");
}

TEST_F(LessThanTest, InfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "Infinity", "0");
}

TEST_F(LessThanTest, InfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "Infinity", "0");
}

TEST_F(LessThanTest, NegativeInfinityWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "abc");
}

TEST_F(LessThanTest, NegativeInfinityWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "abc");
}

TEST_F(LessThanTest, NegativeInfinityWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", " ");
}

TEST_F(LessThanTest, NegativeInfinityWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", " ");
}

TEST_F(LessThanTest, NegativeInfinityWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "");
}

TEST_F(LessThanTest, NegativeInfinityWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "");
}

TEST_F(LessThanTest, NegativeInfinityWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "-Infinity", "0");
}

TEST_F(LessThanTest, NegativeInfinityWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "-Infinity", "0");
}

TEST_F(LessThanTest, NaNWithStringAbc)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "abc");
}

TEST_F(LessThanTest, NaNWithStringAbc_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "abc");
}

TEST_F(LessThanTest, NaNWithStringSpace)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", " ");
}

TEST_F(LessThanTest, NaNWithStringSpace_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", " ");
}

TEST_F(LessThanTest, NaNWithEmptyString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "");
}

TEST_F(LessThanTest, NaNWithEmptyString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "");
}

TEST_F(LessThanTest, NaNWithZeroString)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, false, "NaN", "0");
}

TEST_F(LessThanTest, NaNWithZeroString_Const)
{
    ASSERT_BOOL_OP2(m_utils, CodeGenTestUtils::OpType::CmpLT, true, "NaN", "0");
}
