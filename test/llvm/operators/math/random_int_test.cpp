#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

using ::testing::Return;

class LLVMRandomIntTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMRandomIntTest, IntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, -45, 12);
}

TEST_F(LLVMRandomIntTest, IntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, -45, 12);
}

TEST_F(LLVMRandomIntTest, DoubleAsInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, -45.0, 12.0);
}

TEST_F(LLVMRandomIntTest, DoubleAsInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, -45.0, 12.0);
}

TEST_F(LLVMRandomIntTest, TruncatedDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randint(12, 6)).Times(2).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, 12, 6.05);
}

TEST_F(LLVMRandomIntTest, TruncatedDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(12, 6)).Times(2).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, 12, 6.05);
}

TEST_F(LLVMRandomIntTest, NegativeDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-78, -45)).Times(2).WillRepeatedly(Return(-59));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, -78.686, -45);
}

TEST_F(LLVMRandomIntTest, NegativeDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-78, -45)).Times(2).WillRepeatedly(Return(-59));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, -78.686, -45);
}

TEST_F(LLVMRandomIntTest, StringIntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, "-45", "12");
}

TEST_F(LLVMRandomIntTest, StringIntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, "-45", "12");
}

TEST_F(LLVMRandomIntTest, StringDoubleToInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, "-45.0", "12");
}

TEST_F(LLVMRandomIntTest, StringDoubleToInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, "-45.0", "12");
}

TEST_F(LLVMRandomIntTest, StringIntegerToDouble)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-15));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, "-45", "12.0");
}

TEST_F(LLVMRandomIntTest, StringIntegerToDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-15));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, "-45", "12.0");
}

TEST_F(LLVMRandomIntTest, BooleanRange)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, false, true);
}

TEST_F(LLVMRandomIntTest, BooleanRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, false, true);
}

TEST_F(LLVMRandomIntTest, BooleanWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, true, 5);
}

TEST_F(LLVMRandomIntTest, BooleanWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, true, 5);
}

TEST_F(LLVMRandomIntTest, NumberWithBoolean)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, false, 8, false);
}

TEST_F(LLVMRandomIntTest, NumberWithBoolean_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::RandomInt, true, 8, false);
}

// NOTE: Infinity, -Infinity and NaN behavior is undefined
