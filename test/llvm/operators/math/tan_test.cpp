#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMTanTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMTanTest, FortyFiveDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 45.0).toDouble(), 1.0);
}

TEST_F(LLVMTanTest, FortyFiveDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 45.0).toDouble(), 1.0);
}

TEST_F(LLVMTanTest, NinetyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 90.0).toDouble(), inf);
}

TEST_F(LLVMTanTest, NinetyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 90.0).toDouble(), inf);
}

TEST_F(LLVMTanTest, TwoSeventyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 270.0).toDouble(), -inf);
}

TEST_F(LLVMTanTest, TwoSeventyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 270.0).toDouble(), -inf);
}

TEST_F(LLVMTanTest, FourFiftyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 450.0).toDouble(), inf);
}

TEST_F(LLVMTanTest, FourFiftyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 450.0).toDouble(), inf);
}

TEST_F(LLVMTanTest, NegativeNinetyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, -90.0).toDouble(), -inf);
}

TEST_F(LLVMTanTest, NegativeNinetyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, -90.0).toDouble(), -inf);
}

TEST_F(LLVMTanTest, NegativeTwoSeventyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, -270.0).toDouble(), inf);
}

TEST_F(LLVMTanTest, NegativeTwoSeventyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, -270.0).toDouble(), inf);
}

TEST_F(LLVMTanTest, NegativeFourFiftyDegrees)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, -450.0).toDouble(), -inf);
}

TEST_F(LLVMTanTest, NegativeFourFiftyDegrees_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, -450.0).toDouble(), -inf);
}

TEST_F(LLVMTanTest, OneEightyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 180.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, OneEightyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 180.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, NegativeOneEightyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, -180.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, NegativeOneEightyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, -180.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, VerySmallAngle)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 2.87e-9).toDouble(), 1e-10);
}

TEST_F(LLVMTanTest, VerySmallAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 2.87e-9).toDouble(), 1e-10);
}

TEST_F(LLVMTanTest, EvenSmallerAngle)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 2.8647e-9).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, EvenSmallerAngle_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 2.8647e-9).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, -0.0).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, inf).isNaN());
}

TEST_F(LLVMTanTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, inf).isNaN());
}

TEST_F(LLVMTanTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, -inf).isNaN());
}

TEST_F(LLVMTanTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, -inf).isNaN());
}

TEST_F(LLVMTanTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, false, nan).toDouble(), 0.0);
}

TEST_F(LLVMTanTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Tan, true, nan).toDouble(), 0.0);
}
