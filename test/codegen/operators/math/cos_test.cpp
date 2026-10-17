#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class CosTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(CosTest, SixtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, 60.0).toDouble(), 0.5);
}

TEST_F(CosTest, SixtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, 60.0).toDouble(), 0.5);
}

TEST_F(CosTest, NinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, 90.0).toDouble(), 0.0);
}

TEST_F(CosTest, NinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, 90.0).toDouble(), 0.0);
}

TEST_F(CosTest, SixHundredDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, 600.0).toDouble(), -0.5);
}

TEST_F(CosTest, SixHundredDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, 600.0).toDouble(), -0.5);
}

TEST_F(CosTest, AlmostNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, 89.9999999971352).toDouble(), 1e-10);
}

TEST_F(CosTest, AlmostNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, 89.9999999971352).toDouble(), 1e-10);
}

TEST_F(CosTest, VeryCloseToNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, 89.999999999).toDouble(), 0.0);
}

TEST_F(CosTest, VeryCloseToNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, 89.999999999).toDouble(), 0.0);
}

TEST_F(CosTest, NegativeSixtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, -60.0).toDouble(), 0.5);
}

TEST_F(CosTest, NegativeSixtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, -60.0).toDouble(), 0.5);
}

TEST_F(CosTest, NegativeNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, -90.0).toDouble(), 0.0);
}

TEST_F(CosTest, NegativeNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, -90.0).toDouble(), 0.0);
}

TEST_F(CosTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, 0.0).toDouble(), 1.0);
}

TEST_F(CosTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, 0.0).toDouble(), 1.0);
}

TEST_F(CosTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, -0.0).toDouble(), 1.0);
}

TEST_F(CosTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, -0.0).toDouble(), 1.0);
}

TEST_F(CosTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, inf).isNaN());
}

TEST_F(CosTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, inf).isNaN());
}

TEST_F(CosTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, -inf).isNaN());
}

TEST_F(CosTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, -inf).isNaN());
}

TEST_F(CosTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, false, nan).toDouble(), 1.0);
}

TEST_F(CosTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Cos, true, nan).toDouble(), 1.0);
}
