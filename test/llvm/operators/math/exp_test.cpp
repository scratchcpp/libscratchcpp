#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMExpTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMExpTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, 1.0).toDouble(), std::exp(1.0));
}

TEST_F(LLVMExpTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, 1.0).toDouble(), std::exp(1.0));
}

TEST_F(LLVMExpTest, ZeroPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, 0.5).toDouble(), std::exp(0.5));
}

TEST_F(LLVMExpTest, ZeroPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, 0.5).toDouble(), std::exp(0.5));
}

TEST_F(LLVMExpTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, 0.0).toDouble(), 1.0);
}

TEST_F(LLVMExpTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, 0.0).toDouble(), 1.0);
}

TEST_F(LLVMExpTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, -0.0).toDouble(), 1.0);
}

TEST_F(LLVMExpTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, -0.0).toDouble(), 1.0);
}

TEST_F(LLVMExpTest, NegativeZeroPointSeven)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, -0.7).toDouble(), std::exp(-0.7));
}

TEST_F(LLVMExpTest, NegativeZeroPointSeven_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, -0.7).toDouble(), std::exp(-0.7));
}

TEST_F(LLVMExpTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, inf).toDouble(), inf);
}

TEST_F(LLVMExpTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, inf).toDouble(), inf);
}

TEST_F(LLVMExpTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, -inf).toDouble(), 0.0);
}

TEST_F(LLVMExpTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, -inf).toDouble(), 0.0);
}

TEST_F(LLVMExpTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, false, nan).toDouble(), 1.0);
}

TEST_F(LLVMExpTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp, true, nan).toDouble(), 1.0);
}
