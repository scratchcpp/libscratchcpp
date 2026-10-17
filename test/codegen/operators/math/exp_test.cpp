#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class ExpTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(ExpTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, 1.0).toDouble(), std::exp(1.0));
}

TEST_F(ExpTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, 1.0).toDouble(), std::exp(1.0));
}

TEST_F(ExpTest, ZeroPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, 0.5).toDouble(), std::exp(0.5));
}

TEST_F(ExpTest, ZeroPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, 0.5).toDouble(), std::exp(0.5));
}

TEST_F(ExpTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, 0.0).toDouble(), 1.0);
}

TEST_F(ExpTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, 0.0).toDouble(), 1.0);
}

TEST_F(ExpTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, -0.0).toDouble(), 1.0);
}

TEST_F(ExpTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, -0.0).toDouble(), 1.0);
}

TEST_F(ExpTest, NegativeZeroPointSeven)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, -0.7).toDouble(), std::exp(-0.7));
}

TEST_F(ExpTest, NegativeZeroPointSeven_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, -0.7).toDouble(), std::exp(-0.7));
}

TEST_F(ExpTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, inf).toDouble(), inf);
}

TEST_F(ExpTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, inf).toDouble(), inf);
}

TEST_F(ExpTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, -inf).toDouble(), 0.0);
}

TEST_F(ExpTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, -inf).toDouble(), 0.0);
}

TEST_F(ExpTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, false, nan).toDouble(), 1.0);
}

TEST_F(ExpTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp, true, nan).toDouble(), 1.0);
}
