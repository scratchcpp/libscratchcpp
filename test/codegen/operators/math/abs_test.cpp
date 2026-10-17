#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class AbsTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(AbsTest, PositiveFour)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, 4.0).toDouble(), 4.0);
}

TEST_F(AbsTest, PositiveFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, 4.0).toDouble(), 4.0);
}

TEST_F(AbsTest, PositiveDecimal)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, 3.2).toDouble(), 3.2);
}

TEST_F(AbsTest, PositiveDecimal_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, 3.2).toDouble(), 3.2);
}

TEST_F(AbsTest, NegativeTwo)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, -2.0).toDouble(), 2.0);
}

TEST_F(AbsTest, NegativeTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, -2.0).toDouble(), 2.0);
}

TEST_F(AbsTest, NegativeDecimalTwoPointFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, -2.5).toDouble(), 2.5);
}

TEST_F(AbsTest, NegativeDecimalTwoPointFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, -2.5).toDouble(), 2.5);
}

TEST_F(AbsTest, NegativeDecimalTwoPointSix)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, -2.6).toDouble(), 2.6);
}

TEST_F(AbsTest, NegativeDecimalTwoPointSix_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, -2.6).toDouble(), 2.6);
}

TEST_F(AbsTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, 0.0).toDouble(), 0.0);
}

TEST_F(AbsTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, 0.0).toDouble(), 0.0);
}

TEST_F(AbsTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, -0.0).toDouble(), 0.0);
}

TEST_F(AbsTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, -0.0).toDouble(), 0.0);
}

TEST_F(AbsTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, inf).toDouble(), inf);
}

TEST_F(AbsTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, inf).toDouble(), inf);
}

TEST_F(AbsTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, -inf).toDouble(), inf);
}

TEST_F(AbsTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, -inf).toDouble(), inf);
}

TEST_F(AbsTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, false, nan).toDouble(), 0);
}

TEST_F(AbsTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Abs, true, nan).toDouble(), 0);
}
