#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class Log10Test : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(Log10Test, Ten)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, 10.0).toDouble(), 1.0);
}

TEST_F(Log10Test, Ten_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, 10.0).toDouble(), 1.0);
}

TEST_F(Log10Test, OneThousand)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, 1000.0).toDouble(), 3.0);
}

TEST_F(Log10Test, OneThousand_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, 1000.0).toDouble(), 3.0);
}

TEST_F(Log10Test, ZeroPointZeroOne)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, 0.01).toDouble(), -2.0);
}

TEST_F(Log10Test, ZeroPointZeroOne_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, 0.01).toDouble(), -2.0);
}

TEST_F(Log10Test, PositiveZero)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, 0.0).toDouble(), -inf);
}

TEST_F(Log10Test, PositiveZero_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, 0.0).toDouble(), -inf);
}

TEST_F(Log10Test, NegativeZero)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, -0.0).toDouble(), -inf);
}

TEST_F(Log10Test, NegativeZero_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, -0.0).toDouble(), -inf);
}

TEST_F(Log10Test, NegativeNumber)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, -0.7).isNaN());
}

TEST_F(Log10Test, NegativeNumber_Const)
{
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, -0.7).isNaN());
}

TEST_F(Log10Test, PositiveInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, inf).toDouble(), inf);
}

TEST_F(Log10Test, PositiveInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, inf).toDouble(), inf);
}

TEST_F(Log10Test, NegativeInfinity)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, -inf).isNaN());
}

TEST_F(Log10Test, NegativeInfinity_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, -inf).isNaN());
}

TEST_F(Log10Test, NaN)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, false, nan).toDouble(), -inf);
}

TEST_F(Log10Test, NaN_Const)
{
    static const double inf = std::numeric_limits<double>::infinity();
    static const double nan = std::numeric_limits<double>::quiet_NaN();
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::Log10, true, nan).toDouble(), -inf);
}
