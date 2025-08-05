#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMLnTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMLnTest, ExpOfOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, std::exp(1.0)).toDouble(), 1.0);
}

TEST_F(LLVMLnTest, ExpOfOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, std::exp(1.0)).toDouble(), 1.0);
}

TEST_F(LLVMLnTest, ExpOfTwo)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, std::exp(2.0)).toDouble(), 2.0);
}

TEST_F(LLVMLnTest, ExpOfTwo_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, std::exp(2.0)).toDouble(), 2.0);
}

TEST_F(LLVMLnTest, ExpOfZeroPointThree)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, std::exp(0.3)).toDouble() * 100) / 100, 0.3);
}

TEST_F(LLVMLnTest, ExpOfZeroPointThree_Const)
{
    ASSERT_EQ(std::round(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, std::exp(0.3)).toDouble() * 100) / 100, 0.3);
}

TEST_F(LLVMLnTest, One)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, 1.0).toDouble(), 0.0);
}

TEST_F(LLVMLnTest, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, 1.0).toDouble(), 0.0);
}

TEST_F(LLVMLnTest, PositiveZero)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, 0.0).toDouble(), -inf);
}

TEST_F(LLVMLnTest, PositiveZero_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, 0.0).toDouble(), -inf);
}

TEST_F(LLVMLnTest, NegativeZero)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, -0.0).toDouble(), -inf);
}

TEST_F(LLVMLnTest, NegativeZero_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, -0.0).toDouble(), -inf);
}

TEST_F(LLVMLnTest, NegativeNumber)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, -0.7).isNaN());
}

TEST_F(LLVMLnTest, NegativeNumber_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, -0.7).isNaN());
}

TEST_F(LLVMLnTest, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, inf).toDouble(), inf);
}

TEST_F(LLVMLnTest, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, inf).toDouble(), inf);
}

TEST_F(LLVMLnTest, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, -inf).isNaN());
}

TEST_F(LLVMLnTest, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, -inf).isNaN());
}

TEST_F(LLVMLnTest, NaN)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, false, nan).toDouble(), -inf);
}

TEST_F(LLVMLnTest, NaN_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Ln, true, nan).toDouble(), -inf);
}
