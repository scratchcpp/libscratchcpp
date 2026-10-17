#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class AcosTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(AcosTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, 1.0).toDouble(), 0.0);
}

TEST_F(AcosTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, 1.0).toDouble(), 0.0);
}

TEST_F(AcosTest, OneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, 0.5).toDouble() * 100) / 100, 60.0);
}

TEST_F(AcosTest, OneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, 0.5).toDouble() * 100) / 100, 60.0);
}

TEST_F(AcosTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, 0.0).toDouble(), 90.0);
}

TEST_F(AcosTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, 0.0).toDouble(), 90.0);
}

TEST_F(AcosTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, -0.0).toDouble(), 90.0);
}

TEST_F(AcosTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, -0.0).toDouble(), 90.0);
}

TEST_F(AcosTest, NegativeOneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, -0.5).toDouble() * 100) / 100, 120.0);
}

TEST_F(AcosTest, NegativeOneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, -0.5).toDouble() * 100) / 100, 120.0);
}

TEST_F(AcosTest, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, -1.0).toDouble(), 180.0);
}

TEST_F(AcosTest, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, -1.0).toDouble(), 180.0);
}

TEST_F(AcosTest, OutOfRangePositive)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, 1.1).isNaN());
}

TEST_F(AcosTest, OutOfRangePositive_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, 1.1).isNaN());
}

TEST_F(AcosTest, OutOfRangeNegative)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, -1.2).isNaN());
}

TEST_F(AcosTest, OutOfRangeNegative_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, -1.2).isNaN());
}

TEST_F(AcosTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, inf).isNaN());
}

TEST_F(AcosTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, inf).isNaN());
}

TEST_F(AcosTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, -inf).isNaN());
}

TEST_F(AcosTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, -inf).isNaN());
}

TEST_F(AcosTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, false, nan).toDouble(), 90.0);
}

TEST_F(AcosTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Acos, true, nan).toDouble(), 90.0);
}
