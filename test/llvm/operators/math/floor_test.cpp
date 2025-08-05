#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMFloorTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMFloorTest, FourPointZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, 4.0).toDouble(), 4.0);
}

TEST_F(LLVMFloorTest, FourPointZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, 4.0).toDouble(), 4.0);
}

TEST_F(LLVMFloorTest, ThreePointTwo)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, 3.2).toDouble(), 3.0);
}

TEST_F(LLVMFloorTest, ThreePointTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, 3.2).toDouble(), 3.0);
}

TEST_F(LLVMFloorTest, ThreePointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, 3.5).toDouble(), 3.0);
}

TEST_F(LLVMFloorTest, ThreePointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, 3.5).toDouble(), 3.0);
}

TEST_F(LLVMFloorTest, ThreePointSix)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, 3.6).toDouble(), 3.0);
}

TEST_F(LLVMFloorTest, ThreePointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, 3.6).toDouble(), 3.0);
}

TEST_F(LLVMFloorTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMFloorTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMFloorTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -0.0).toDouble(), -0.0);
}

TEST_F(LLVMFloorTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -0.0).toDouble(), -0.0);
}

TEST_F(LLVMFloorTest, NegativeTwoPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -2.4).toDouble(), -3.0);
}

TEST_F(LLVMFloorTest, NegativeTwoPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -2.4).toDouble(), -3.0);
}

TEST_F(LLVMFloorTest, NegativeTwoPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -2.5).toDouble(), -3.0);
}

TEST_F(LLVMFloorTest, NegativeTwoPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -2.5).toDouble(), -3.0);
}

TEST_F(LLVMFloorTest, NegativeTwoPointSix)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -2.6).toDouble(), -3.0);
}

TEST_F(LLVMFloorTest, NegativeTwoPointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -2.6).toDouble(), -3.0);
}

TEST_F(LLVMFloorTest, NegativeZeroPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -0.4).toDouble(), -1.0);
}

TEST_F(LLVMFloorTest, NegativeZeroPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -0.4).toDouble(), -1.0);
}

TEST_F(LLVMFloorTest, NegativeZeroPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -0.5).toDouble(), -1.0);
}

TEST_F(LLVMFloorTest, NegativeZeroPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -0.5).toDouble(), -1.0);
}

TEST_F(LLVMFloorTest, NegativeZeroPointFiftyOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -0.51).toDouble(), -1.0);
}

TEST_F(LLVMFloorTest, NegativeZeroPointFiftyOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -0.51).toDouble(), -1.0);
}

TEST_F(LLVMFloorTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, inf).toDouble(), inf);
}

TEST_F(LLVMFloorTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, inf).toDouble(), inf);
}

TEST_F(LLVMFloorTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, -inf).toDouble(), -inf);
}

TEST_F(LLVMFloorTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, -inf).toDouble(), -inf);
}

TEST_F(LLVMFloorTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, false, nan).toDouble(), 0);
}

TEST_F(LLVMFloorTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Floor, true, nan).toDouble(), 0);
}
