#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class Exp10Test : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(Exp10Test, One)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, 1.0).toDouble(), 10.0);
}

TEST_F(Exp10Test, One_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, 1.0).toDouble(), 10.0);
}

TEST_F(Exp10Test, Three)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, 3.0).toDouble(), 1000.0);
}

TEST_F(Exp10Test, Three_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, 3.0).toDouble(), 1000.0);
}

TEST_F(Exp10Test, PositiveZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, 0.0).toDouble(), 1.0);
}

TEST_F(Exp10Test, PositiveZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, 0.0).toDouble(), 1.0);
}

TEST_F(Exp10Test, NegativeZero)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, -0.0).toDouble(), 1.0);
}

TEST_F(Exp10Test, NegativeZero_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, -0.0).toDouble(), 1.0);
}

TEST_F(Exp10Test, NegativeOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, -1.0).toDouble(), 0.1);
}

TEST_F(Exp10Test, NegativeOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, -1.0).toDouble(), 0.1);
}

TEST_F(Exp10Test, NegativeFive)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, -5.0).toDouble(), 0.00001);
}

TEST_F(Exp10Test, NegativeFive_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, -5.0).toDouble(), 0.00001);
}

TEST_F(Exp10Test, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, inf).toDouble(), inf);
}

TEST_F(Exp10Test, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, inf).toDouble(), inf);
}

TEST_F(Exp10Test, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, -inf).toDouble(), 0.0);
}

TEST_F(Exp10Test, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, -inf).toDouble(), 0.0);
}

TEST_F(Exp10Test, NaN)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, false, nan).toDouble(), 1.0);
}

TEST_F(Exp10Test, NaN_Const)
{
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Exp10, true, nan).toDouble(), 1.0);
}
