#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMAtanTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMAtanTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, false, 1.0).toDouble(), 45.0);
}

TEST_F(LLVMAtanTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, true, 1.0).toDouble(), 45.0);
}

TEST_F(LLVMAtanTest, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, false, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMAtanTest, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, true, 0.0).toDouble(), 0.0);
}

TEST_F(LLVMAtanTest, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, false, -0.0).toDouble(), -0.0);
}

TEST_F(LLVMAtanTest, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, true, -0.0).toDouble(), -0.0);
}

TEST_F(LLVMAtanTest, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, false, -1.0).toDouble(), -45.0);
}

TEST_F(LLVMAtanTest, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, true, -1.0).toDouble(), -45.0);
}

TEST_F(LLVMAtanTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, false, inf).toDouble(), 90.0);
}

TEST_F(LLVMAtanTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, true, inf).toDouble(), 90.0);
}

TEST_F(LLVMAtanTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, false, -inf).toDouble(), -90.0);
}

TEST_F(LLVMAtanTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, true, -inf).toDouble(), -90.0);
}

TEST_F(LLVMAtanTest, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, false, nan).toDouble(), 0.0);
}

TEST_F(LLVMAtanTest, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Atan, true, nan).toDouble(), 0.0);
}
