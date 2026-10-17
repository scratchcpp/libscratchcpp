#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class AtanTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(AtanTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, false, 1.0).toDouble(), 45.0);
}

TEST_F(AtanTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, true, 1.0).toDouble(), 45.0);
}

TEST_F(AtanTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, false, 0.0).toDouble(), 0.0);
}

TEST_F(AtanTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, true, 0.0).toDouble(), 0.0);
}

TEST_F(AtanTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, false, -0.0).toDouble(), -0.0);
}

TEST_F(AtanTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, true, -0.0).toDouble(), -0.0);
}

TEST_F(AtanTest, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, false, -1.0).toDouble(), -45.0);
}

TEST_F(AtanTest, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, true, -1.0).toDouble(), -45.0);
}

TEST_F(AtanTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, false, inf).toDouble(), 90.0);
}

TEST_F(AtanTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, true, inf).toDouble(), 90.0);
}

TEST_F(AtanTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, false, -inf).toDouble(), -90.0);
}

TEST_F(AtanTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, true, -inf).toDouble(), -90.0);
}

TEST_F(AtanTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, false, nan).toDouble(), 0.0);
}

TEST_F(AtanTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Atan, true, nan).toDouble(), 0.0);
}
