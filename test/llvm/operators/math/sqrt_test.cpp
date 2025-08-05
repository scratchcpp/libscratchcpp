#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMSqrtTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMSqrtTest, SixteenPointZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, 16.0).toDouble(), 4.0);
}

TEST_F(LLVMSqrtTest, SixteenPointZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, 16.0).toDouble(), 4.0);
}

TEST_F(LLVMSqrtTest, ZeroPointZeroFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, 0.04).toDouble(), 0.2);
}

TEST_F(LLVMSqrtTest, ZeroPointZeroFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, 0.04).toDouble(), 0.2);
}

TEST_F(LLVMSqrtTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMSqrtTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMSqrtTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMSqrtTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMSqrtTest, NegativeFour)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, -4.0).isNaN());
}

TEST_F(LLVMSqrtTest, NegativeFour_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, -4.0).isNaN());
}

TEST_F(LLVMSqrtTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, inf).toDouble(), inf);
}

TEST_F(LLVMSqrtTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, inf).toDouble(), inf);
}

TEST_F(LLVMSqrtTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, -inf).isNaN());
}

TEST_F(LLVMSqrtTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, -inf).isNaN());
}

TEST_F(LLVMSqrtTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, false, nan).toDouble(), 0);
}

TEST_F(LLVMSqrtTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sqrt, true, nan).toDouble(), 0);
}
