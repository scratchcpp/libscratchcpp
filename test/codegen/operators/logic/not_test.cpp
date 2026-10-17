#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class NotTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(NotTest, PositiveNumber)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, 10);
}

TEST_F(NotTest, PositiveNumber_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, 10);
}

TEST_F(NotTest, NegativeDecimal)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, -4.25);
}

TEST_F(NotTest, NegativeDecimal_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, -4.25);
}

TEST_F(NotTest, PositiveDecimal)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, 5.312);
}

TEST_F(NotTest, PositiveDecimal_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, 5.312);
}

TEST_F(NotTest, One)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, 1);
}

TEST_F(NotTest, One_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, 1);
}

TEST_F(NotTest, Zero)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, 0);
}

TEST_F(NotTest, Zero_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, 0);
}

TEST_F(NotTest, TrueBoolean)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, true);
}

TEST_F(NotTest, TrueBoolean_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, true);
}

TEST_F(NotTest, FalseBoolean)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, false);
}

TEST_F(NotTest, FalseBoolean_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, false);
}

TEST_F(NotTest, AbcString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "abc");
}

TEST_F(NotTest, AbcString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "abc");
}

TEST_F(NotTest, NumericString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "5.25");
}

TEST_F(NotTest, NumericString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "5.25");
}

TEST_F(NotTest, ZeroString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "0");
}

TEST_F(NotTest, ZeroString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "0");
}

TEST_F(NotTest, Infinity)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "Infinity");
}

TEST_F(NotTest, Infinity_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "Infinity");
}

TEST_F(NotTest, NegativeInfinity)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "-Infinity");
}

TEST_F(NotTest, NegativeInfinity_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "-Infinity");
}

TEST_F(NotTest, NaN)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "NaN");
}

TEST_F(NotTest, NaN_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "NaN");
}

TEST_F(NotTest, TrueString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "true");
}

TEST_F(NotTest, TrueString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "true");
}

TEST_F(NotTest, FalseString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "false");
}

TEST_F(NotTest, FalseString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "false");
}

TEST_F(NotTest, UppercaseTrueString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "TRUE");
}

TEST_F(NotTest, UppercaseTrueString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "TRUE");
}

TEST_F(NotTest, UppercaseFalseString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "FALSE");
}

TEST_F(NotTest, UppercaseFalseString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "FALSE");
}

TEST_F(NotTest, ZeroPaddedOne)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "00001");
}

TEST_F(NotTest, ZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "00001");
}

TEST_F(NotTest, ZeroPaddedZero)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "00000");
}

TEST_F(NotTest, ZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "00000");
}

TEST_F(NotTest, InfinityString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "Infinity");
}

TEST_F(NotTest, InfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "Infinity");
}

TEST_F(NotTest, LowercaseInfinityString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "infinity");
}

TEST_F(NotTest, LowercaseInfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "infinity");
}

TEST_F(NotTest, NegativeInfinityString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "-Infinity");
}

TEST_F(NotTest, NegativeInfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "-Infinity");
}

TEST_F(NotTest, LowercaseNegativeInfinityString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "-infinity");
}

TEST_F(NotTest, LowercaseNegativeInfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "-infinity");
}

TEST_F(NotTest, NaNString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "NaN");
}

TEST_F(NotTest, NaNString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "NaN");
}

TEST_F(NotTest, LowercaseNaNString)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, false, "nan");
}

TEST_F(NotTest, LowercaseNaNString_Const)
{
    ASSERT_BOOL_OP1(m_utils, CodeGenTestUtils::OpType::Not, true, "nan");
}
