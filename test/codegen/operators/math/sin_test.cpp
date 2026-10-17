#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class SinTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(SinTest, ThirtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, 30.0).toDouble(), 0.5);
}

TEST_F(SinTest, ThirtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, 30.0).toDouble(), 0.5);
}

TEST_F(SinTest, NinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, 90.0).toDouble(), 1.0);
}

TEST_F(SinTest, NinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, 90.0).toDouble(), 1.0);
}

TEST_F(SinTest, VerySmallAngle)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, 2.8e-9).toDouble(), 0.0);
}

TEST_F(SinTest, VerySmallAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, 2.8e-9).toDouble(), 0.0);
}

TEST_F(SinTest, SlightlyLargerSmallAngle)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, 2.9e-9).toDouble(), 1e-10);
}

TEST_F(SinTest, SlightlyLargerSmallAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, 2.9e-9).toDouble(), 1e-10);
}

TEST_F(SinTest, FiveSeventyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, 570.0).toDouble(), -0.5);
}

TEST_F(SinTest, FiveSeventyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, 570.0).toDouble(), -0.5);
}

TEST_F(SinTest, NegativeThirtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, -30.0).toDouble(), -0.5);
}

TEST_F(SinTest, NegativeThirtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, -30.0).toDouble(), -0.5);
}

TEST_F(SinTest, NegativeNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, -90.0).toDouble(), -1.0);
}

TEST_F(SinTest, NegativeNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, -90.0).toDouble(), -1.0);
}

TEST_F(SinTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, 0.0).toDouble(), 0.0);
}

TEST_F(SinTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, 0.0).toDouble(), 0.0);
}

TEST_F(SinTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, -0.0).toDouble(), 0.0);
}

TEST_F(SinTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, -0.0).toDouble(), 0.0);
}

TEST_F(SinTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, inf).isNaN());
}

TEST_F(SinTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, inf).isNaN());
}

TEST_F(SinTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, -inf).isNaN());
}

TEST_F(SinTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, -inf).isNaN());
}

TEST_F(SinTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, false, nan).toDouble(), 0);
}

TEST_F(SinTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sin, true, nan).toDouble(), 0);
}
