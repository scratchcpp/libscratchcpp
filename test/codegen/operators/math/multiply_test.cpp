#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class MultiplyTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(MultiplyTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, 50, 2);
}

TEST_F(MultiplyTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, 50, 2);
}

TEST_F(MultiplyTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, -500, 25);
}

TEST_F(MultiplyTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, -500, 25);
}

TEST_F(MultiplyTest, StringNegativeWithNegative)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "-500", -25);
}

TEST_F(MultiplyTest, StringNegativeWithNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "-500", -25);
}

TEST_F(MultiplyTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "2.54", "6.28");
}

TEST_F(MultiplyTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "2.54", "6.28");
}

TEST_F(MultiplyTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, true, true);
}

TEST_F(MultiplyTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, true, true);
}

TEST_F(MultiplyTest, InfinityTimesInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "Infinity", "Infinity");
}

TEST_F(MultiplyTest, InfinityTimesInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "Infinity", "Infinity");
}

TEST_F(MultiplyTest, InfinityTimesZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "Infinity", 0);
}

TEST_F(MultiplyTest, InfinityTimesZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "Infinity", 0);
}

TEST_F(MultiplyTest, InfinityTimesPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "Infinity", 2);
}

TEST_F(MultiplyTest, InfinityTimesPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "Infinity", 2);
}

TEST_F(MultiplyTest, InfinityTimesNegative)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "Infinity", -2);
}

TEST_F(MultiplyTest, InfinityTimesNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "Infinity", -2);
}

TEST_F(MultiplyTest, InfinityTimesNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "Infinity", "-Infinity");
}

TEST_F(MultiplyTest, InfinityTimesNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "Infinity", "-Infinity");
}

TEST_F(MultiplyTest, NegativeInfinityTimesInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "-Infinity", "Infinity");
}

TEST_F(MultiplyTest, NegativeInfinityTimesInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "-Infinity", "Infinity");
}

TEST_F(MultiplyTest, NegativeInfinityTimesZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "-Infinity", 0);
}

TEST_F(MultiplyTest, NegativeInfinityTimesZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "-Infinity", 0);
}

TEST_F(MultiplyTest, NegativeInfinityTimesPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "-Infinity", 2);
}

TEST_F(MultiplyTest, NegativeInfinityTimesPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "-Infinity", 2);
}

TEST_F(MultiplyTest, NegativeInfinityTimesNegative)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "-Infinity", -2);
}

TEST_F(MultiplyTest, NegativeInfinityTimesNegative_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "-Infinity", -2);
}

TEST_F(MultiplyTest, NegativeInfinityTimesNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "-Infinity", "-Infinity");
}

TEST_F(MultiplyTest, NegativeInfinityTimesNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "-Infinity", "-Infinity");
}

TEST_F(MultiplyTest, NumberTimesNaN)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, 1, "NaN");
}

TEST_F(MultiplyTest, NumberTimesNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, 1, "NaN");
}

TEST_F(MultiplyTest, NaNTimesNumber)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, false, "NaN", 1);
}

TEST_F(MultiplyTest, NaNTimesNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mul, true, "NaN", 1);
}
