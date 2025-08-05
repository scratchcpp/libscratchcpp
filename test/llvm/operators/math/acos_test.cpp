#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMAcosTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMAcosTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, 1.0).toDouble(), 0.0);
}

TEST_F(LLVMAcosTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, 1.0).toDouble(), 0.0);
}

TEST_F(LLVMAcosTest, OneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, 0.5).toDouble() * 100) / 100, 60.0);
}

TEST_F(LLVMAcosTest, OneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, 0.5).toDouble() * 100) / 100, 60.0);
}

TEST_F(LLVMAcosTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, 0.0).toDouble(), 90.0);
}

TEST_F(LLVMAcosTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, 0.0).toDouble(), 90.0);
}

TEST_F(LLVMAcosTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, -0.0).toDouble(), 90.0);
}

TEST_F(LLVMAcosTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, -0.0).toDouble(), 90.0);
}

TEST_F(LLVMAcosTest, NegativeOneHalf)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, -0.5).toDouble() * 100) / 100, 120.0);
}

TEST_F(LLVMAcosTest, NegativeOneHalf_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, -0.5).toDouble() * 100) / 100, 120.0);
}

TEST_F(LLVMAcosTest, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, -1.0).toDouble(), 180.0);
}

TEST_F(LLVMAcosTest, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, -1.0).toDouble(), 180.0);
}

TEST_F(LLVMAcosTest, OutOfRangePositive)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, 1.1).isNaN());
}

TEST_F(LLVMAcosTest, OutOfRangePositive_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, 1.1).isNaN());
}

TEST_F(LLVMAcosTest, OutOfRangeNegative)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, -1.2).isNaN());
}

TEST_F(LLVMAcosTest, OutOfRangeNegative_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, -1.2).isNaN());
}

TEST_F(LLVMAcosTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, inf).isNaN());
}

TEST_F(LLVMAcosTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, inf).isNaN());
}

TEST_F(LLVMAcosTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, -inf).isNaN());
}

TEST_F(LLVMAcosTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, -inf).isNaN());
}

TEST_F(LLVMAcosTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, false, nan).toDouble(), 90.0);
}

TEST_F(LLVMAcosTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Acos, true, nan).toDouble(), 90.0);
}
