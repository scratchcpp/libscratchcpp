#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMCosTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMCosTest, SixtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, 60.0).toDouble(), 0.5);
}

TEST_F(LLVMCosTest, SixtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, 60.0).toDouble(), 0.5);
}

TEST_F(LLVMCosTest, NinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, 90.0).toDouble(), 0.0);
}

TEST_F(LLVMCosTest, NinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, 90.0).toDouble(), 0.0);
}

TEST_F(LLVMCosTest, SixHundredDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, 600.0).toDouble(), -0.5);
}

TEST_F(LLVMCosTest, SixHundredDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, 600.0).toDouble(), -0.5);
}

TEST_F(LLVMCosTest, AlmostNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, 89.9999999971352).toDouble(), 1e-10);
}

TEST_F(LLVMCosTest, AlmostNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, 89.9999999971352).toDouble(), 1e-10);
}

TEST_F(LLVMCosTest, VeryCloseToNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, 89.999999999).toDouble(), 0.0);
}

TEST_F(LLVMCosTest, VeryCloseToNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, 89.999999999).toDouble(), 0.0);
}

TEST_F(LLVMCosTest, NegativeSixtyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, -60.0).toDouble(), 0.5);
}

TEST_F(LLVMCosTest, NegativeSixtyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, -60.0).toDouble(), 0.5);
}

TEST_F(LLVMCosTest, NegativeNinetyDegrees)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, -90.0).toDouble(), 0.0);
}

TEST_F(LLVMCosTest, NegativeNinetyDegrees_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, -90.0).toDouble(), 0.0);
}

TEST_F(LLVMCosTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, 0.0).toDouble(), 1.0);
}

TEST_F(LLVMCosTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, 0.0).toDouble(), 1.0);
}

TEST_F(LLVMCosTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, -0.0).toDouble(), 1.0);
}

TEST_F(LLVMCosTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, -0.0).toDouble(), 1.0);
}

TEST_F(LLVMCosTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, inf).isNaN());
}

TEST_F(LLVMCosTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, inf).isNaN());
}

TEST_F(LLVMCosTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, -inf).isNaN());
}

TEST_F(LLVMCosTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, -inf).isNaN());
}

TEST_F(LLVMCosTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, false, nan).toDouble(), 1.0);
}

TEST_F(LLVMCosTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Cos, true, nan).toDouble(), 1.0);
}
