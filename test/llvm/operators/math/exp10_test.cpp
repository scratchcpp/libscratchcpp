#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMExp10Test : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMExp10Test, One)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, 1.0).toDouble(), 10.0);
}

TEST_F(LLVMExp10Test, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, 1.0).toDouble(), 10.0);
}

TEST_F(LLVMExp10Test, Three)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, 3.0).toDouble(), 1000.0);
}

TEST_F(LLVMExp10Test, Three_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, 3.0).toDouble(), 1000.0);
}

TEST_F(LLVMExp10Test, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, 0.0).toDouble(), 1.0);
}

TEST_F(LLVMExp10Test, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, 0.0).toDouble(), 1.0);
}

TEST_F(LLVMExp10Test, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, -0.0).toDouble(), 1.0);
}

TEST_F(LLVMExp10Test, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, -0.0).toDouble(), 1.0);
}

TEST_F(LLVMExp10Test, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, -1.0).toDouble(), 0.1);
}

TEST_F(LLVMExp10Test, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, -1.0).toDouble(), 0.1);
}

TEST_F(LLVMExp10Test, NegativeFive)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, -5.0).toDouble(), 0.00001);
}

TEST_F(LLVMExp10Test, NegativeFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, -5.0).toDouble(), 0.00001);
}

TEST_F(LLVMExp10Test, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, inf).toDouble(), inf);
}

TEST_F(LLVMExp10Test, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, inf).toDouble(), inf);
}

TEST_F(LLVMExp10Test, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, -inf).toDouble(), 0.0);
}

TEST_F(LLVMExp10Test, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, -inf).toDouble(), 0.0);
}

TEST_F(LLVMExp10Test, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, false, nan).toDouble(), 1.0);
}

TEST_F(LLVMExp10Test, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::Exp10, true, nan).toDouble(), 1.0);
}
