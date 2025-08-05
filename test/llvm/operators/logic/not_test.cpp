#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMNotTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMNotTest, PositiveNumber)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, 10);
}

TEST_F(LLVMNotTest, PositiveNumber_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, 10);
}

TEST_F(LLVMNotTest, NegativeDecimal)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, -4.25);
}

TEST_F(LLVMNotTest, NegativeDecimal_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, -4.25);
}

TEST_F(LLVMNotTest, PositiveDecimal)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, 5.312);
}

TEST_F(LLVMNotTest, PositiveDecimal_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, 5.312);
}

TEST_F(LLVMNotTest, One)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, 1);
}

TEST_F(LLVMNotTest, One_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, 1);
}

TEST_F(LLVMNotTest, Zero)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, 0);
}

TEST_F(LLVMNotTest, Zero_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, 0);
}

TEST_F(LLVMNotTest, TrueBoolean)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, true);
}

TEST_F(LLVMNotTest, TrueBoolean_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, true);
}

TEST_F(LLVMNotTest, FalseBoolean)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, false);
}

TEST_F(LLVMNotTest, FalseBoolean_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, false);
}

TEST_F(LLVMNotTest, AbcString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "abc");
}

TEST_F(LLVMNotTest, AbcString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "abc");
}

TEST_F(LLVMNotTest, NumericString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "5.25");
}

TEST_F(LLVMNotTest, NumericString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "5.25");
}

TEST_F(LLVMNotTest, ZeroString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "0");
}

TEST_F(LLVMNotTest, ZeroString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "0");
}

TEST_F(LLVMNotTest, Infinity)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "Infinity");
}

TEST_F(LLVMNotTest, Infinity_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "Infinity");
}

TEST_F(LLVMNotTest, NegativeInfinity)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "-Infinity");
}

TEST_F(LLVMNotTest, NegativeInfinity_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "-Infinity");
}

TEST_F(LLVMNotTest, NaN)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "NaN");
}

TEST_F(LLVMNotTest, NaN_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "NaN");
}

TEST_F(LLVMNotTest, TrueString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "true");
}

TEST_F(LLVMNotTest, TrueString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "true");
}

TEST_F(LLVMNotTest, FalseString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "false");
}

TEST_F(LLVMNotTest, FalseString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "false");
}

TEST_F(LLVMNotTest, UppercaseTrueString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "TRUE");
}

TEST_F(LLVMNotTest, UppercaseTrueString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "TRUE");
}

TEST_F(LLVMNotTest, UppercaseFalseString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "FALSE");
}

TEST_F(LLVMNotTest, UppercaseFalseString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "FALSE");
}

TEST_F(LLVMNotTest, ZeroPaddedOne)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "00001");
}

TEST_F(LLVMNotTest, ZeroPaddedOne_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "00001");
}

TEST_F(LLVMNotTest, ZeroPaddedZero)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "00000");
}

TEST_F(LLVMNotTest, ZeroPaddedZero_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "00000");
}

TEST_F(LLVMNotTest, InfinityString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "Infinity");
}

TEST_F(LLVMNotTest, InfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "Infinity");
}

TEST_F(LLVMNotTest, LowercaseInfinityString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "infinity");
}

TEST_F(LLVMNotTest, LowercaseInfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "infinity");
}

TEST_F(LLVMNotTest, NegativeInfinityString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "-Infinity");
}

TEST_F(LLVMNotTest, NegativeInfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "-Infinity");
}

TEST_F(LLVMNotTest, LowercaseNegativeInfinityString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "-infinity");
}

TEST_F(LLVMNotTest, LowercaseNegativeInfinityString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "-infinity");
}

TEST_F(LLVMNotTest, NaNString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "NaN");
}

TEST_F(LLVMNotTest, NaNString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "NaN");
}

TEST_F(LLVMNotTest, LowercaseNaNString)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, false, "nan");
}

TEST_F(LLVMNotTest, LowercaseNaNString_Const)
{
    ASSERT_BOOL_OP1(m_utils, LLVMTestUtils::OpType::Not, true, "nan");
}
