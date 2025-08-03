#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMAddTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMAddTest, PositiveIntegers)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, 50, 25);
}

TEST_F(LLVMAddTest, PositiveIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, 50, 25);
}

TEST_F(LLVMAddTest, NegativeWithPositive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, -500, 25);
}

TEST_F(LLVMAddTest, NegativeWithPositive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, -500, 25);
}

TEST_F(LLVMAddTest, NegativeIntegers)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, -500, -25);
}

TEST_F(LLVMAddTest, NegativeIntegers_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, -500, -25);
}

TEST_F(LLVMAddTest, StringDecimals)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, "2.54", "6.28");
}

TEST_F(LLVMAddTest, StringDecimals_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, "2.54", "6.28");
}

TEST_F(LLVMAddTest, MixedDecimalAndString)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, 2.54, "-6.28");
}

TEST_F(LLVMAddTest, MixedDecimalAndString_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, 2.54, "-6.28");
}

TEST_F(LLVMAddTest, BooleanValues)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, true, true);
}

TEST_F(LLVMAddTest, BooleanValues_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, true, true);
}

TEST_F(LLVMAddTest, InfinityPlusInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, "Infinity", "Infinity");
}

TEST_F(LLVMAddTest, InfinityPlusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, "Infinity", "Infinity");
}

TEST_F(LLVMAddTest, InfinityPlusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, "Infinity", "-Infinity");
}

TEST_F(LLVMAddTest, InfinityPlusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, "Infinity", "-Infinity");
}

TEST_F(LLVMAddTest, NegativeInfinityPlusInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, "-Infinity", "Infinity");
}

TEST_F(LLVMAddTest, NegativeInfinityPlusInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, "-Infinity", "Infinity");
}

TEST_F(LLVMAddTest, NegativeInfinityPlusNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMAddTest, NegativeInfinityPlusNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMAddTest, NumberPlusNaN)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, 1, "NaN");
}

TEST_F(LLVMAddTest, NumberPlusNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, 1, "NaN");
}

TEST_F(LLVMAddTest, NaNPlusNumber)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, false, "NaN", 1);
}

TEST_F(LLVMAddTest, NaNPlusNumber_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Add, true, "NaN", 1);
}
