#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class AddTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(AddTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, 50, 25);
}

TEST_F(AddTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, 50, 25);
}

TEST_F(AddTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, -500, 25);
}

TEST_F(AddTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, -500, 25);
}

TEST_F(AddTest, NegativeIntegers)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, -500, -25);
}

TEST_F(AddTest, NegativeIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, -500, -25);
}

TEST_F(AddTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, "2.54", "6.28");
}

TEST_F(AddTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, "2.54", "6.28");
}

TEST_F(AddTest, MixedDecimalAndString)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, 2.54, "-6.28");
}

TEST_F(AddTest, MixedDecimalAndString_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, 2.54, "-6.28");
}

TEST_F(AddTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, true, true);
}

TEST_F(AddTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, true, true);
}

TEST_F(AddTest, InfinityPlusInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, "Infinity", "Infinity");
}

TEST_F(AddTest, InfinityPlusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, "Infinity", "Infinity");
}

TEST_F(AddTest, InfinityPlusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, "Infinity", "-Infinity");
}

TEST_F(AddTest, InfinityPlusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, "Infinity", "-Infinity");
}

TEST_F(AddTest, NegativeInfinityPlusInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, "-Infinity", "Infinity");
}

TEST_F(AddTest, NegativeInfinityPlusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, "-Infinity", "Infinity");
}

TEST_F(AddTest, NegativeInfinityPlusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, "-Infinity", "-Infinity");
}

TEST_F(AddTest, NegativeInfinityPlusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, "-Infinity", "-Infinity");
}

TEST_F(AddTest, NumberPlusNaN)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, 1, "NaN");
}

TEST_F(AddTest, NumberPlusNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, 1, "NaN");
}

TEST_F(AddTest, NaNPlusNumber)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, false, "NaN", 1);
}

TEST_F(AddTest, NaNPlusNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Add, true, "NaN", 1);
}
