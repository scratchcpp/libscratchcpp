#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class TanTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(TanTest, FortyFiveDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 45.0).toDouble(), 1.0);
}

TEST_F(TanTest, FortyFiveDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 45.0).toDouble(), 1.0);
}

TEST_F(TanTest, NinetyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 90.0).toDouble(), inf);
}

TEST_F(TanTest, NinetyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 90.0).toDouble(), inf);
}

TEST_F(TanTest, TwoSeventyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 270.0).toDouble(), -inf);
}

TEST_F(TanTest, TwoSeventyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 270.0).toDouble(), -inf);
}

TEST_F(TanTest, FourFiftyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 450.0).toDouble(), inf);
}

TEST_F(TanTest, FourFiftyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 450.0).toDouble(), inf);
}

TEST_F(TanTest, NegativeNinetyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, -90.0).toDouble(), -inf);
}

TEST_F(TanTest, NegativeNinetyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, -90.0).toDouble(), -inf);
}

TEST_F(TanTest, NegativeTwoSeventyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, -270.0).toDouble(), inf);
}

TEST_F(TanTest, NegativeTwoSeventyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, -270.0).toDouble(), inf);
}

TEST_F(TanTest, NegativeFourFiftyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, -450.0).toDouble(), -inf);
}

TEST_F(TanTest, NegativeFourFiftyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, -450.0).toDouble(), -inf);
}

TEST_F(TanTest, OneEightyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 180.0).toDouble(), 0.0);
}

TEST_F(TanTest, OneEightyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 180.0).toDouble(), 0.0);
}

TEST_F(TanTest, NegativeOneEightyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, -180.0).toDouble(), 0.0);
}

TEST_F(TanTest, NegativeOneEightyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, -180.0).toDouble(), 0.0);
}

TEST_F(TanTest, VerySmallAngle)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 2.87e-9).toDouble(), 1e-10);
}

TEST_F(TanTest, VerySmallAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 2.87e-9).toDouble(), 1e-10);
}

TEST_F(TanTest, EvenSmallerAngle)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 2.8647e-9).toDouble(), 0.0);
}

TEST_F(TanTest, EvenSmallerAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 2.8647e-9).toDouble(), 0.0);
}

TEST_F(TanTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, 0.0).toDouble(), 0.0);
}

TEST_F(TanTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, 0.0).toDouble(), 0.0);
}

TEST_F(TanTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, -0.0).toDouble(), 0.0);
}

TEST_F(TanTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, -0.0).toDouble(), 0.0);
}

TEST_F(TanTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, inf).isNaN());
}

TEST_F(TanTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, inf).isNaN());
}

TEST_F(TanTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, -inf).isNaN());
}

TEST_F(TanTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, -inf).isNaN());
}

TEST_F(TanTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, false, nan).toDouble(), 0.0);
}

TEST_F(TanTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Tan, true, nan).toDouble(), 0.0);
}
