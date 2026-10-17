#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class CeilTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(CeilTest, EightPointZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, 8.0).toDouble(), 8.0);
}

TEST_F(CeilTest, EightPointZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, 8.0).toDouble(), 8.0);
}

TEST_F(CeilTest, ThreePointTwo)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, 3.2).toDouble(), 4.0);
}

TEST_F(CeilTest, ThreePointTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, 3.2).toDouble(), 4.0);
}

TEST_F(CeilTest, ThreePointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, 3.5).toDouble(), 4.0);
}

TEST_F(CeilTest, ThreePointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, 3.5).toDouble(), 4.0);
}

TEST_F(CeilTest, ThreePointSix)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, 3.6).toDouble(), 4.0);
}

TEST_F(CeilTest, ThreePointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, 3.6).toDouble(), 4.0);
}

TEST_F(CeilTest, ZeroPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, 0.4).toDouble(), 1.0);
}

TEST_F(CeilTest, ZeroPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, 0.4).toDouble(), 1.0);
}

TEST_F(CeilTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, 0.0).toDouble(), 0.0);
}

TEST_F(CeilTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, 0.0).toDouble(), 0.0);
}

TEST_F(CeilTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -0.0).toDouble(), -0.0);
}

TEST_F(CeilTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -0.0).toDouble(), -0.0);
}

TEST_F(CeilTest, NegativeTwoPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -2.4).toDouble(), -2.0);
}

TEST_F(CeilTest, NegativeTwoPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -2.4).toDouble(), -2.0);
}

TEST_F(CeilTest, NegativeTwoPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -2.5).toDouble(), -2.0);
}

TEST_F(CeilTest, NegativeTwoPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -2.5).toDouble(), -2.0);
}

TEST_F(CeilTest, NegativeTwoPointSix)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -2.6).toDouble(), -2.0);
}

TEST_F(CeilTest, NegativeTwoPointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -2.6).toDouble(), -2.0);
}

TEST_F(CeilTest, NegativeZeroPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -0.4).toDouble(), -0.0);
}

TEST_F(CeilTest, NegativeZeroPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -0.4).toDouble(), -0.0);
}

TEST_F(CeilTest, NegativeZeroPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -0.5).toDouble(), -0.0);
}

TEST_F(CeilTest, NegativeZeroPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -0.5).toDouble(), -0.0);
}

TEST_F(CeilTest, NegativeZeroPointFiftyOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -0.51).toDouble(), -0.0);
}

TEST_F(CeilTest, NegativeZeroPointFiftyOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -0.51).toDouble(), -0.0);
}

TEST_F(CeilTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, inf).toDouble(), inf);
}

TEST_F(CeilTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, inf).toDouble(), inf);
}

TEST_F(CeilTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, -inf).toDouble(), -inf);
}

TEST_F(CeilTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, -inf).toDouble(), -inf);
}

TEST_F(CeilTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, false, nan).toDouble(), 0);
}

TEST_F(CeilTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ceil, true, nan).toDouble(), 0);
}
