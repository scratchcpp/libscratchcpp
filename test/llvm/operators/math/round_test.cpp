#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMRoundTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMRoundTest, FourPointZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, 4.0).toDouble(), 4.0);
}

TEST_F(LLVMRoundTest, FourPointZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, 4.0).toDouble(), 4.0);
}

TEST_F(LLVMRoundTest, ThreePointTwo)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, 3.2).toDouble(), 3.0);
}

TEST_F(LLVMRoundTest, ThreePointTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, 3.2).toDouble(), 3.0);
}

TEST_F(LLVMRoundTest, ThreePointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, 3.5).toDouble(), 4.0);
}

TEST_F(LLVMRoundTest, ThreePointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, 3.5).toDouble(), 4.0);
}

TEST_F(LLVMRoundTest, ThreePointSix)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, 3.6).toDouble(), 4.0);
}

TEST_F(LLVMRoundTest, ThreePointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, 3.6).toDouble(), 4.0);
}

TEST_F(LLVMRoundTest, NegativeTwoPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, -2.4).toDouble(), -2.0);
}

TEST_F(LLVMRoundTest, NegativeTwoPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, -2.4).toDouble(), -2.0);
}

TEST_F(LLVMRoundTest, NegativeTwoPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, -2.5).toDouble(), -2.0);
}

TEST_F(LLVMRoundTest, NegativeTwoPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, -2.5).toDouble(), -2.0);
}

TEST_F(LLVMRoundTest, NegativeTwoPointSix)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, -2.6).toDouble(), -3.0);
}

TEST_F(LLVMRoundTest, NegativeTwoPointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, -2.6).toDouble(), -3.0);
}

TEST_F(LLVMRoundTest, NegativeZeroPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, -0.4).toDouble(), -0.0);
}

TEST_F(LLVMRoundTest, NegativeZeroPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, -0.4).toDouble(), -0.0);
}

TEST_F(LLVMRoundTest, NegativeZeroPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, -0.5).toDouble(), -0.0);
}

TEST_F(LLVMRoundTest, NegativeZeroPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, -0.5).toDouble(), -0.0);
}

TEST_F(LLVMRoundTest, NegativeZeroPointFiftyOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, -0.51).toDouble(), -1.0);
}

TEST_F(LLVMRoundTest, NegativeZeroPointFiftyOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, -0.51).toDouble(), -1.0);
}

TEST_F(LLVMRoundTest, Infinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, inf).toDouble(), inf);
}

TEST_F(LLVMRoundTest, Infinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, inf).toDouble(), inf);
}

TEST_F(LLVMRoundTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, -inf).toDouble(), -inf);
}

TEST_F(LLVMRoundTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, -inf).toDouble(), -inf);
}

TEST_F(LLVMRoundTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, false, nan).toDouble(), 0);
}

TEST_F(LLVMRoundTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Round, true, nan).toDouble(), 0);
}
