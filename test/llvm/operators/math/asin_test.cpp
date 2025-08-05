#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMAsinTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMAsinTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, 1.0).toDouble(), 90.0);
}

TEST_F(LLVMAsinTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, 1.0).toDouble(), 90.0);
}

TEST_F(LLVMAsinTest, OneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, 0.5).toDouble() * 100) / 100, 30.0);
}

TEST_F(LLVMAsinTest, OneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, 0.5).toDouble() * 100) / 100, 30.0);
}

TEST_F(LLVMAsinTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMAsinTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMAsinTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMAsinTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMAsinTest, NegativeOneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, -0.5).toDouble() * 100) / 100, -30.0);
}

TEST_F(LLVMAsinTest, NegativeOneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, -0.5).toDouble() * 100) / 100, -30.0);
}

TEST_F(LLVMAsinTest, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, -1.0).toDouble(), -90.0);
}

TEST_F(LLVMAsinTest, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, -1.0).toDouble(), -90.0);
}

TEST_F(LLVMAsinTest, OutOfRangePositive)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, 1.1).isNaN());
}

TEST_F(LLVMAsinTest, OutOfRangePositive_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, 1.1).isNaN());
}

TEST_F(LLVMAsinTest, OutOfRangeNegative)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, -1.2).isNaN());
}

TEST_F(LLVMAsinTest, OutOfRangeNegative_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, -1.2).isNaN());
}

TEST_F(LLVMAsinTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, inf).isNaN());
}

TEST_F(LLVMAsinTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, inf).isNaN());
}

TEST_F(LLVMAsinTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, -inf).isNaN());
}

TEST_F(LLVMAsinTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, -inf).isNaN());
}

TEST_F(LLVMAsinTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, false, nan).toDouble(), 0.0);
}

TEST_F(LLVMAsinTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Asin, true, nan).toDouble(), 0.0);
}
