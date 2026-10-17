#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class FloorTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(FloorTest, FourPointZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, 4.0).toDouble(), 4.0);
}

TEST_F(FloorTest, FourPointZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, 4.0).toDouble(), 4.0);
}

TEST_F(FloorTest, ThreePointTwo)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, 3.2).toDouble(), 3.0);
}

TEST_F(FloorTest, ThreePointTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, 3.2).toDouble(), 3.0);
}

TEST_F(FloorTest, ThreePointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, 3.5).toDouble(), 3.0);
}

TEST_F(FloorTest, ThreePointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, 3.5).toDouble(), 3.0);
}

TEST_F(FloorTest, ThreePointSix)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, 3.6).toDouble(), 3.0);
}

TEST_F(FloorTest, ThreePointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, 3.6).toDouble(), 3.0);
}

TEST_F(FloorTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, 0.0).toDouble(), 0.0);
}

TEST_F(FloorTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, 0.0).toDouble(), 0.0);
}

TEST_F(FloorTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -0.0).toDouble(), -0.0);
}

TEST_F(FloorTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -0.0).toDouble(), -0.0);
}

TEST_F(FloorTest, NegativeTwoPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -2.4).toDouble(), -3.0);
}

TEST_F(FloorTest, NegativeTwoPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -2.4).toDouble(), -3.0);
}

TEST_F(FloorTest, NegativeTwoPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -2.5).toDouble(), -3.0);
}

TEST_F(FloorTest, NegativeTwoPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -2.5).toDouble(), -3.0);
}

TEST_F(FloorTest, NegativeTwoPointSix)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -2.6).toDouble(), -3.0);
}

TEST_F(FloorTest, NegativeTwoPointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -2.6).toDouble(), -3.0);
}

TEST_F(FloorTest, NegativeZeroPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -0.4).toDouble(), -1.0);
}

TEST_F(FloorTest, NegativeZeroPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -0.4).toDouble(), -1.0);
}

TEST_F(FloorTest, NegativeZeroPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -0.5).toDouble(), -1.0);
}

TEST_F(FloorTest, NegativeZeroPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -0.5).toDouble(), -1.0);
}

TEST_F(FloorTest, NegativeZeroPointFiftyOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -0.51).toDouble(), -1.0);
}

TEST_F(FloorTest, NegativeZeroPointFiftyOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -0.51).toDouble(), -1.0);
}

TEST_F(FloorTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, inf).toDouble(), inf);
}

TEST_F(FloorTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, inf).toDouble(), inf);
}

TEST_F(FloorTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, -inf).toDouble(), -inf);
}

TEST_F(FloorTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, -inf).toDouble(), -inf);
}

TEST_F(FloorTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, false, nan).toDouble(), 0);
}

TEST_F(FloorTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Floor, true, nan).toDouble(), 0);
}
