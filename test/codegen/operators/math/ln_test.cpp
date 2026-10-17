#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class LnTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(LnTest, ExpOfOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, std::exp(1.0)).toDouble(), 1.0);
}

TEST_F(LnTest, ExpOfOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, std::exp(1.0)).toDouble(), 1.0);
}

TEST_F(LnTest, ExpOfTwo)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, std::exp(2.0)).toDouble(), 2.0);
}

TEST_F(LnTest, ExpOfTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, std::exp(2.0)).toDouble(), 2.0);
}

TEST_F(LnTest, ExpOfZeroPointThree)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, std::exp(0.3)).toDouble() * 100) / 100, 0.3);
}

TEST_F(LnTest, ExpOfZeroPointThree_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, std::exp(0.3)).toDouble() * 100) / 100, 0.3);
}

TEST_F(LnTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, 1.0).toDouble(), 0.0);
}

TEST_F(LnTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, 1.0).toDouble(), 0.0);
}

TEST_F(LnTest, PositiveZero)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, 0.0).toDouble(), -inf);
}

TEST_F(LnTest, PositiveZero_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, 0.0).toDouble(), -inf);
}

TEST_F(LnTest, NegativeZero)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, -0.0).toDouble(), -inf);
}

TEST_F(LnTest, NegativeZero_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, -0.0).toDouble(), -inf);
}

TEST_F(LnTest, NegativeNumber)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, -0.7).isNaN());
}

TEST_F(LnTest, NegativeNumber_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, -0.7).isNaN());
}

TEST_F(LnTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, inf).toDouble(), inf);
}

TEST_F(LnTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, inf).toDouble(), inf);
}

TEST_F(LnTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, -inf).isNaN());
}

TEST_F(LnTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, -inf).isNaN());
}

TEST_F(LnTest, NaN)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, false, nan).toDouble(), -inf);
}

TEST_F(LnTest, NaN_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Ln, true, nan).toDouble(), -inf);
}
