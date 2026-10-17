#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class DivideTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(DivideTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 50, 2);
}

TEST_F(DivideTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 50, 2);
}

TEST_F(DivideTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, -500, 25);
}

TEST_F(DivideTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, -500, 25);
}

TEST_F(DivideTest, StringNegativeWithNegative)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "-500", -25);
}

TEST_F(DivideTest, StringNegativeWithNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "-500", -25);
}

TEST_F(DivideTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "3.5", "2.5");
}

TEST_F(DivideTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "3.5", "2.5");
}

TEST_F(DivideTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, true, true);
}

TEST_F(DivideTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, true, true);
}

TEST_F(DivideTest, InfinityDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "Infinity", "Infinity");
}

TEST_F(DivideTest, InfinityDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "Infinity", "Infinity");
}

TEST_F(DivideTest, InfinityDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "Infinity", 0);
}

TEST_F(DivideTest, InfinityDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "Infinity", 0);
}

TEST_F(DivideTest, InfinityDividedByPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "Infinity", 2);
}

TEST_F(DivideTest, InfinityDividedByPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "Infinity", 2);
}

TEST_F(DivideTest, InfinityDividedByNegative)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "Infinity", -2);
}

TEST_F(DivideTest, InfinityDividedByNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "Infinity", -2);
}

TEST_F(DivideTest, InfinityDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "Infinity", "-Infinity");
}

TEST_F(DivideTest, InfinityDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "Infinity", "-Infinity");
}

TEST_F(DivideTest, NegativeInfinityDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "-Infinity", "Infinity");
}

TEST_F(DivideTest, NegativeInfinityDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "-Infinity", "Infinity");
}

TEST_F(DivideTest, NegativeInfinityDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "-Infinity", 0);
}

TEST_F(DivideTest, NegativeInfinityDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "-Infinity", 0);
}

TEST_F(DivideTest, NegativeInfinityDividedByPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "-Infinity", 2);
}

TEST_F(DivideTest, NegativeInfinityDividedByPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "-Infinity", 2);
}

TEST_F(DivideTest, NegativeInfinityDividedByNegative)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "-Infinity", -2);
}

TEST_F(DivideTest, NegativeInfinityDividedByNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "-Infinity", -2);
}

TEST_F(DivideTest, NegativeInfinityDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "-Infinity", "-Infinity");
}

TEST_F(DivideTest, NegativeInfinityDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "-Infinity", "-Infinity");
}

TEST_F(DivideTest, ZeroDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 0, "Infinity");
}

TEST_F(DivideTest, ZeroDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 0, "Infinity");
}

TEST_F(DivideTest, PositiveDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 2, "Infinity");
}

TEST_F(DivideTest, PositiveDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 2, "Infinity");
}

TEST_F(DivideTest, NegativeDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, -2, "Infinity");
}

TEST_F(DivideTest, NegativeDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, -2, "Infinity");
}

TEST_F(DivideTest, ZeroDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 0, "-Infinity");
}

TEST_F(DivideTest, ZeroDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 0, "-Infinity");
}

TEST_F(DivideTest, PositiveDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 2, "-Infinity");
}

TEST_F(DivideTest, PositiveDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 2, "-Infinity");
}

TEST_F(DivideTest, NegativeDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, -2, "-Infinity");
}

TEST_F(DivideTest, NegativeDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, -2, "-Infinity");
}

TEST_F(DivideTest, NumberDividedByNaN)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 1, "NaN");
}

TEST_F(DivideTest, NumberDividedByNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 1, "NaN");
}

TEST_F(DivideTest, NaNDividedByNumber)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, "NaN", 1);
}

TEST_F(DivideTest, NaNDividedByNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, "NaN", 1);
}

TEST_F(DivideTest, PositiveDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 5, 0);
}

TEST_F(DivideTest, PositiveDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 5, 0);
}

TEST_F(DivideTest, NegativeDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, -5, 0);
}

TEST_F(DivideTest, NegativeDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, -5, 0);
}

TEST_F(DivideTest, ZeroDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, false, 0, 0);
}

TEST_F(DivideTest, ZeroDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Div, true, 0, 0);
}
