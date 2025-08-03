#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMAbsTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMAbsTest, PositiveFour)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, 4.0).toDouble(), 4.0);
}

TEST_F(LLVMAbsTest, PositiveFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, 4.0).toDouble(), 4.0);
}

TEST_F(LLVMAbsTest, PositiveDecimal)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, 3.2).toDouble(), 3.2);
}

TEST_F(LLVMAbsTest, PositiveDecimal_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, 3.2).toDouble(), 3.2);
}

TEST_F(LLVMAbsTest, NegativeTwo)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, -2.0).toDouble(), 2.0);
}

TEST_F(LLVMAbsTest, NegativeTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, -2.0).toDouble(), 2.0);
}

TEST_F(LLVMAbsTest, NegativeDecimalTwoPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, -2.5).toDouble(), 2.5);
}

TEST_F(LLVMAbsTest, NegativeDecimalTwoPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, -2.5).toDouble(), 2.5);
}

TEST_F(LLVMAbsTest, NegativeDecimalTwoPointSix)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, -2.6).toDouble(), 2.6);
}

TEST_F(LLVMAbsTest, NegativeDecimalTwoPointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, -2.6).toDouble(), 2.6);
}

TEST_F(LLVMAbsTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMAbsTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMAbsTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMAbsTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMAbsTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, inf).toDouble(), inf);
}

TEST_F(LLVMAbsTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, inf).toDouble(), inf);
}

TEST_F(LLVMAbsTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, -inf).toDouble(), inf);
}

TEST_F(LLVMAbsTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, -inf).toDouble(), inf);
}

TEST_F(LLVMAbsTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, false, nan).toDouble(), 0);
}

TEST_F(LLVMAbsTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Abs, true, nan).toDouble(), 0);
}
