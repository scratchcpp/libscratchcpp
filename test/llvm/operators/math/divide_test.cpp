#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMDivideTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMDivideTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 50, 2);
}

TEST_F(LLVMDivideTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 50, 2);
}

TEST_F(LLVMDivideTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, -500, 25);
}

TEST_F(LLVMDivideTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, -500, 25);
}

TEST_F(LLVMDivideTest, StringNegativeWithNegative)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "-500", -25);
}

TEST_F(LLVMDivideTest, StringNegativeWithNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "-500", -25);
}

TEST_F(LLVMDivideTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "3.5", "2.5");
}

TEST_F(LLVMDivideTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "3.5", "2.5");
}

TEST_F(LLVMDivideTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, true, true);
}

TEST_F(LLVMDivideTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, true, true);
}

TEST_F(LLVMDivideTest, InfinityDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "Infinity", "Infinity");
}

TEST_F(LLVMDivideTest, InfinityDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "Infinity", "Infinity");
}

TEST_F(LLVMDivideTest, InfinityDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "Infinity", 0);
}

TEST_F(LLVMDivideTest, InfinityDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "Infinity", 0);
}

TEST_F(LLVMDivideTest, InfinityDividedByPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "Infinity", 2);
}

TEST_F(LLVMDivideTest, InfinityDividedByPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "Infinity", 2);
}

TEST_F(LLVMDivideTest, InfinityDividedByNegative)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "Infinity", -2);
}

TEST_F(LLVMDivideTest, InfinityDividedByNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "Infinity", -2);
}

TEST_F(LLVMDivideTest, InfinityDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "Infinity", "-Infinity");
}

TEST_F(LLVMDivideTest, InfinityDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "Infinity", "-Infinity");
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "-Infinity", "Infinity");
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "-Infinity", "Infinity");
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "-Infinity", 0);
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "-Infinity", 0);
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "-Infinity", 2);
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "-Infinity", 2);
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByNegative)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "-Infinity", -2);
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "-Infinity", -2);
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMDivideTest, NegativeInfinityDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMDivideTest, ZeroDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 0, "Infinity");
}

TEST_F(LLVMDivideTest, ZeroDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 0, "Infinity");
}

TEST_F(LLVMDivideTest, PositiveDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 2, "Infinity");
}

TEST_F(LLVMDivideTest, PositiveDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 2, "Infinity");
}

TEST_F(LLVMDivideTest, NegativeDividedByInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, -2, "Infinity");
}

TEST_F(LLVMDivideTest, NegativeDividedByInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, -2, "Infinity");
}

TEST_F(LLVMDivideTest, ZeroDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 0, "-Infinity");
}

TEST_F(LLVMDivideTest, ZeroDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 0, "-Infinity");
}

TEST_F(LLVMDivideTest, PositiveDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 2, "-Infinity");
}

TEST_F(LLVMDivideTest, PositiveDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 2, "-Infinity");
}

TEST_F(LLVMDivideTest, NegativeDividedByNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, -2, "-Infinity");
}

TEST_F(LLVMDivideTest, NegativeDividedByNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, -2, "-Infinity");
}

TEST_F(LLVMDivideTest, NumberDividedByNaN)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 1, "NaN");
}

TEST_F(LLVMDivideTest, NumberDividedByNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 1, "NaN");
}

TEST_F(LLVMDivideTest, NaNDividedByNumber)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, "NaN", 1);
}

TEST_F(LLVMDivideTest, NaNDividedByNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, "NaN", 1);
}

TEST_F(LLVMDivideTest, PositiveDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 5, 0);
}

TEST_F(LLVMDivideTest, PositiveDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 5, 0);
}

TEST_F(LLVMDivideTest, NegativeDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, -5, 0);
}

TEST_F(LLVMDivideTest, NegativeDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, -5, 0);
}

TEST_F(LLVMDivideTest, ZeroDividedByZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, false, 0, 0);
}

TEST_F(LLVMDivideTest, ZeroDividedByZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Div, true, 0, 0);
}
