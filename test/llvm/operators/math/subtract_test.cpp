#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMSubtractTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMSubtractTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, 50, 25);
}

TEST_F(LLVMSubtractTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, 50, 25);
}

TEST_F(LLVMSubtractTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, -500, 25);
}

TEST_F(LLVMSubtractTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, -500, 25);
}

TEST_F(LLVMSubtractTest, NegativeIntegers)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, -500, -25);
}

TEST_F(LLVMSubtractTest, NegativeIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, -500, -25);
}

TEST_F(LLVMSubtractTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, "2.54", "6.28");
}

TEST_F(LLVMSubtractTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, "2.54", "6.28");
}

TEST_F(LLVMSubtractTest, MixedDecimalAndString)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, 2.54, "-6.28");
}

TEST_F(LLVMSubtractTest, MixedDecimalAndString_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, 2.54, "-6.28");
}

TEST_F(LLVMSubtractTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, true, true);
}

TEST_F(LLVMSubtractTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, true, true);
}

TEST_F(LLVMSubtractTest, InfinityMinusInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, "Infinity", "Infinity");
}

TEST_F(LLVMSubtractTest, InfinityMinusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, "Infinity", "Infinity");
}

TEST_F(LLVMSubtractTest, InfinityMinusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, "Infinity", "-Infinity");
}

TEST_F(LLVMSubtractTest, InfinityMinusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, "Infinity", "-Infinity");
}

TEST_F(LLVMSubtractTest, NegativeInfinityMinusInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, "-Infinity", "Infinity");
}

TEST_F(LLVMSubtractTest, NegativeInfinityMinusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, "-Infinity", "Infinity");
}

TEST_F(LLVMSubtractTest, NegativeInfinityMinusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMSubtractTest, NegativeInfinityMinusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMSubtractTest, NumberMinusNaN)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, 1, "NaN");
}

TEST_F(LLVMSubtractTest, NumberMinusNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, 1, "NaN");
}

TEST_F(LLVMSubtractTest, NaNMinusNumber)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, false, "NaN", 1);
}

TEST_F(LLVMSubtractTest, NaNMinusNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Sub, true, "NaN", 1);
}
