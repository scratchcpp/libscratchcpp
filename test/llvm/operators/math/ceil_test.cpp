#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMCeilTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMCeilTest, EightPointZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, 8.0).toDouble(), 8.0);
}

TEST_F(LLVMCeilTest, EightPointZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, 8.0).toDouble(), 8.0);
}

TEST_F(LLVMCeilTest, ThreePointTwo)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, 3.2).toDouble(), 4.0);
}

TEST_F(LLVMCeilTest, ThreePointTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, 3.2).toDouble(), 4.0);
}

TEST_F(LLVMCeilTest, ThreePointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, 3.5).toDouble(), 4.0);
}

TEST_F(LLVMCeilTest, ThreePointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, 3.5).toDouble(), 4.0);
}

TEST_F(LLVMCeilTest, ThreePointSix)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, 3.6).toDouble(), 4.0);
}

TEST_F(LLVMCeilTest, ThreePointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, 3.6).toDouble(), 4.0);
}

TEST_F(LLVMCeilTest, ZeroPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, 0.4).toDouble(), 1.0);
}

TEST_F(LLVMCeilTest, ZeroPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, 0.4).toDouble(), 1.0);
}

TEST_F(LLVMCeilTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMCeilTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMCeilTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -0.0).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -0.0).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, NegativeTwoPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -2.4).toDouble(), -2.0);
}

TEST_F(LLVMCeilTest, NegativeTwoPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -2.4).toDouble(), -2.0);
}

TEST_F(LLVMCeilTest, NegativeTwoPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -2.5).toDouble(), -2.0);
}

TEST_F(LLVMCeilTest, NegativeTwoPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -2.5).toDouble(), -2.0);
}

TEST_F(LLVMCeilTest, NegativeTwoPointSix)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -2.6).toDouble(), -2.0);
}

TEST_F(LLVMCeilTest, NegativeTwoPointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -2.6).toDouble(), -2.0);
}

TEST_F(LLVMCeilTest, NegativeZeroPointFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -0.4).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, NegativeZeroPointFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -0.4).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, NegativeZeroPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -0.5).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, NegativeZeroPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -0.5).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, NegativeZeroPointFiftyOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -0.51).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, NegativeZeroPointFiftyOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -0.51).toDouble(), -0.0);
}

TEST_F(LLVMCeilTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, inf).toDouble(), inf);
}

TEST_F(LLVMCeilTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, inf).toDouble(), inf);
}

TEST_F(LLVMCeilTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, -inf).toDouble(), -inf);
}

TEST_F(LLVMCeilTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, -inf).toDouble(), -inf);
}

TEST_F(LLVMCeilTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, false, nan).toDouble(), 0);
}

TEST_F(LLVMCeilTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ceil, true, nan).toDouble(), 0);
}
