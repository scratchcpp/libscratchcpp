#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class SqrtTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(SqrtTest, SixteenPointZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, 16.0).toDouble(), 4.0);
}

TEST_F(SqrtTest, SixteenPointZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, 16.0).toDouble(), 4.0);
}

TEST_F(SqrtTest, ZeroPointZeroFour)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, 0.04).toDouble(), 0.2);
}

TEST_F(SqrtTest, ZeroPointZeroFour_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, 0.04).toDouble(), 0.2);
}

TEST_F(SqrtTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, 0.0).toDouble(), 0.0);
}

TEST_F(SqrtTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, 0.0).toDouble(), 0.0);
}

TEST_F(SqrtTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, -0.0).toDouble(), 0.0);
}

TEST_F(SqrtTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, -0.0).toDouble(), 0.0);
}

TEST_F(SqrtTest, NegativeFour)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, -4.0).isNaN());
}

TEST_F(SqrtTest, NegativeFour_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, -4.0).isNaN());
}

TEST_F(SqrtTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, inf).toDouble(), inf);
}

TEST_F(SqrtTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, inf).toDouble(), inf);
}

TEST_F(SqrtTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, -inf).isNaN());
}

TEST_F(SqrtTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, -inf).isNaN());
}

TEST_F(SqrtTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, false, nan).toDouble(), 0);
}

TEST_F(SqrtTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Sqrt, true, nan).toDouble(), 0);
}
