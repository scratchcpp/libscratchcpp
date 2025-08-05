#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMMultiplyTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMMultiplyTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, 50, 2);
}

TEST_F(LLVMMultiplyTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, 50, 2);
}

TEST_F(LLVMMultiplyTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, -500, 25);
}

TEST_F(LLVMMultiplyTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, -500, 25);
}

TEST_F(LLVMMultiplyTest, StringNegativeWithNegative)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "-500", -25);
}

TEST_F(LLVMMultiplyTest, StringNegativeWithNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "-500", -25);
}

TEST_F(LLVMMultiplyTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "2.54", "6.28");
}

TEST_F(LLVMMultiplyTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "2.54", "6.28");
}

TEST_F(LLVMMultiplyTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, true, true);
}

TEST_F(LLVMMultiplyTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, true, true);
}

TEST_F(LLVMMultiplyTest, InfinityTimesInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "Infinity", "Infinity");
}

TEST_F(LLVMMultiplyTest, InfinityTimesInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "Infinity", "Infinity");
}

TEST_F(LLVMMultiplyTest, InfinityTimesZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "Infinity", 0);
}

TEST_F(LLVMMultiplyTest, InfinityTimesZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "Infinity", 0);
}

TEST_F(LLVMMultiplyTest, InfinityTimesPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "Infinity", 2);
}

TEST_F(LLVMMultiplyTest, InfinityTimesPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "Infinity", 2);
}

TEST_F(LLVMMultiplyTest, InfinityTimesNegative)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "Infinity", -2);
}

TEST_F(LLVMMultiplyTest, InfinityTimesNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "Infinity", -2);
}

TEST_F(LLVMMultiplyTest, InfinityTimesNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "Infinity", "-Infinity");
}

TEST_F(LLVMMultiplyTest, InfinityTimesNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "Infinity", "-Infinity");
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "-Infinity", "Infinity");
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "-Infinity", "Infinity");
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "-Infinity", 0);
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "-Infinity", 0);
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "-Infinity", 2);
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "-Infinity", 2);
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesNegative)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "-Infinity", -2);
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "-Infinity", -2);
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMMultiplyTest, NegativeInfinityTimesNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMMultiplyTest, NumberTimesNaN)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, 1, "NaN");
}

TEST_F(LLVMMultiplyTest, NumberTimesNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, 1, "NaN");
}

TEST_F(LLVMMultiplyTest, NaNTimesNumber)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, false, "NaN", 1);
}

TEST_F(LLVMMultiplyTest, NaNTimesNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mul, true, "NaN", 1);
}
