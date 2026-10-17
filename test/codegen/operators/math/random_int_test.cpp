#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

using ::testing::Return;

class RandomIntTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(RandomIntTest, IntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, -45, 12);
}

TEST_F(RandomIntTest, IntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, -45, 12);
}

TEST_F(RandomIntTest, DoubleAsInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, -45.0, 12.0);
}

TEST_F(RandomIntTest, DoubleAsInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, -45.0, 12.0);
}

TEST_F(RandomIntTest, TruncatedDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randint(12, 6)).Times(2).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, 12, 6.05);
}

TEST_F(RandomIntTest, TruncatedDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(12, 6)).Times(2).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, 12, 6.05);
}

TEST_F(RandomIntTest, NegativeDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-78, -45)).Times(2).WillRepeatedly(Return(-59));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, -78.686, -45);
}

TEST_F(RandomIntTest, NegativeDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-78, -45)).Times(2).WillRepeatedly(Return(-59));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, -78.686, -45);
}

TEST_F(RandomIntTest, StringIntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, "-45", "12");
}

TEST_F(RandomIntTest, StringIntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, "-45", "12");
}

TEST_F(RandomIntTest, StringDoubleToInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, "-45.0", "12");
}

TEST_F(RandomIntTest, StringDoubleToInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, "-45.0", "12");
}

TEST_F(RandomIntTest, StringIntegerToDouble)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-15));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, "-45", "12.0");
}

TEST_F(RandomIntTest, StringIntegerToDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).Times(2).WillRepeatedly(Return(-15));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, "-45", "12.0");
}

TEST_F(RandomIntTest, BooleanRange)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, false, true);
}

TEST_F(RandomIntTest, BooleanRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, false, true);
}

TEST_F(RandomIntTest, BooleanWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, true, 5);
}

TEST_F(RandomIntTest, BooleanWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, true, 5);
}

TEST_F(RandomIntTest, NumberWithBoolean)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, false, 8, false);
}

TEST_F(RandomIntTest, NumberWithBoolean_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).Times(2).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::RandomInt, true, 8, false);
}

// NOTE: Infinity, -Infinity and NaN behavior is undefined
