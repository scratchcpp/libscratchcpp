#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMSinTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMSinTest, ThirtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, 30.0).toDouble(), 0.5);
}

TEST_F(LLVMSinTest, ThirtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, 30.0).toDouble(), 0.5);
}

TEST_F(LLVMSinTest, NinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, 90.0).toDouble(), 1.0);
}

TEST_F(LLVMSinTest, NinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, 90.0).toDouble(), 1.0);
}

TEST_F(LLVMSinTest, VerySmallAngle)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, 2.8e-9).toDouble(), 0.0);
}

TEST_F(LLVMSinTest, VerySmallAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, 2.8e-9).toDouble(), 0.0);
}

TEST_F(LLVMSinTest, SlightlyLargerSmallAngle)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, 2.9e-9).toDouble(), 1e-10);
}

TEST_F(LLVMSinTest, SlightlyLargerSmallAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, 2.9e-9).toDouble(), 1e-10);
}

TEST_F(LLVMSinTest, FiveSeventyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, 570.0).toDouble(), -0.5);
}

TEST_F(LLVMSinTest, FiveSeventyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, 570.0).toDouble(), -0.5);
}

TEST_F(LLVMSinTest, NegativeThirtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, -30.0).toDouble(), -0.5);
}

TEST_F(LLVMSinTest, NegativeThirtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, -30.0).toDouble(), -0.5);
}

TEST_F(LLVMSinTest, NegativeNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, -90.0).toDouble(), -1.0);
}

TEST_F(LLVMSinTest, NegativeNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, -90.0).toDouble(), -1.0);
}

TEST_F(LLVMSinTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMSinTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMSinTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMSinTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMSinTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, inf).isNaN());
}

TEST_F(LLVMSinTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, inf).isNaN());
}

TEST_F(LLVMSinTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, -inf).isNaN());
}

TEST_F(LLVMSinTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, -inf).isNaN());
}

TEST_F(LLVMSinTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, false, nan).toDouble(), 0);
}

TEST_F(LLVMSinTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Sin, true, nan).toDouble(), 0);
}
