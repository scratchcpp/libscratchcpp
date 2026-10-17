#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class SubtractTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(SubtractTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, 50, 25);
}

TEST_F(SubtractTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, 50, 25);
}

TEST_F(SubtractTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, -500, 25);
}

TEST_F(SubtractTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, -500, 25);
}

TEST_F(SubtractTest, NegativeIntegers)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, -500, -25);
}

TEST_F(SubtractTest, NegativeIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, -500, -25);
}

TEST_F(SubtractTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, "2.54", "6.28");
}

TEST_F(SubtractTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, "2.54", "6.28");
}

TEST_F(SubtractTest, MixedDecimalAndString)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, 2.54, "-6.28");
}

TEST_F(SubtractTest, MixedDecimalAndString_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, 2.54, "-6.28");
}

TEST_F(SubtractTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, true, true);
}

TEST_F(SubtractTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, true, true);
}

TEST_F(SubtractTest, InfinityMinusInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, "Infinity", "Infinity");
}

TEST_F(SubtractTest, InfinityMinusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, "Infinity", "Infinity");
}

TEST_F(SubtractTest, InfinityMinusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, "Infinity", "-Infinity");
}

TEST_F(SubtractTest, InfinityMinusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, "Infinity", "-Infinity");
}

TEST_F(SubtractTest, NegativeInfinityMinusInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, "-Infinity", "Infinity");
}

TEST_F(SubtractTest, NegativeInfinityMinusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, "-Infinity", "Infinity");
}

TEST_F(SubtractTest, NegativeInfinityMinusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, "-Infinity", "-Infinity");
}

TEST_F(SubtractTest, NegativeInfinityMinusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, "-Infinity", "-Infinity");
}

TEST_F(SubtractTest, NumberMinusNaN)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, 1, "NaN");
}

TEST_F(SubtractTest, NumberMinusNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, 1, "NaN");
}

TEST_F(SubtractTest, NaNMinusNumber)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, false, "NaN", 1);
}

TEST_F(SubtractTest, NaNMinusNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Sub, true, "NaN", 1);
}
