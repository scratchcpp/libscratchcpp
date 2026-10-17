#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class AsinTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(AsinTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, 1.0).toDouble(), 90.0);
}

TEST_F(AsinTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, 1.0).toDouble(), 90.0);
}

TEST_F(AsinTest, OneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, 0.5).toDouble() * 100) / 100, 30.0);
}

TEST_F(AsinTest, OneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, 0.5).toDouble() * 100) / 100, 30.0);
}

TEST_F(AsinTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, 0.0).toDouble(), 0.0);
}

TEST_F(AsinTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, 0.0).toDouble(), 0.0);
}

TEST_F(AsinTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, -0.0).toDouble(), 0.0);
}

TEST_F(AsinTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, -0.0).toDouble(), 0.0);
}

TEST_F(AsinTest, NegativeOneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, -0.5).toDouble() * 100) / 100, -30.0);
}

TEST_F(AsinTest, NegativeOneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, -0.5).toDouble() * 100) / 100, -30.0);
}

TEST_F(AsinTest, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, -1.0).toDouble(), -90.0);
}

TEST_F(AsinTest, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, -1.0).toDouble(), -90.0);
}

TEST_F(AsinTest, OutOfRangePositive)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, 1.1).isNaN());
}

TEST_F(AsinTest, OutOfRangePositive_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, 1.1).isNaN());
}

TEST_F(AsinTest, OutOfRangeNegative)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, -1.2).isNaN());
}

TEST_F(AsinTest, OutOfRangeNegative_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, -1.2).isNaN());
}

TEST_F(AsinTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, inf).isNaN());
}

TEST_F(AsinTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, inf).isNaN());
}

TEST_F(AsinTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, -inf).isNaN());
}

TEST_F(AsinTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, -inf).isNaN());
}

TEST_F(AsinTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, false, nan).toDouble(), 0.0);
}

TEST_F(AsinTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Asin, true, nan).toDouble(), 0.0);
}
