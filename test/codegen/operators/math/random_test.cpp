#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

using ::testing::Return;

class RandomTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(RandomTest, IntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, -45, 12);
}

TEST_F(RandomTest, IntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, -45, 12);
}

TEST_F(RandomTest, DoubleAsInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, -45.0, 12.0);
}

TEST_F(RandomTest, DoubleAsInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, -45.0, 12.0);
}

TEST_F(RandomTest, DoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(12, 6.05)).WillRepeatedly(Return(3.486789));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, 12, 6.05);
}

TEST_F(RandomTest, DoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(12, 6.05)).WillRepeatedly(Return(3.486789));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, 12, 6.05);
}

TEST_F(RandomTest, NegativeDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-78.686, -45)).WillRepeatedly(Return(-59.468873));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, -78.686, -45);
}

TEST_F(RandomTest, NegativeDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-78.686, -45)).WillRepeatedly(Return(-59.468873));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, -78.686, -45);
}

TEST_F(RandomTest, ReversedDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(-28.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, 6.05, -78.686);
}

TEST_F(RandomTest, ReversedDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(-28.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, 6.05, -78.686);
}

TEST_F(RandomTest, StringIntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-45", "12");
}

TEST_F(RandomTest, StringIntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-45", "12");
}

TEST_F(RandomTest, StringDoubleToInteger)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-45.0", "12");
}

TEST_F(RandomTest, StringDoubleToInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-45.0", "12");
}

TEST_F(RandomTest, StringIntegerToDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-45", "12.0");
}

TEST_F(RandomTest, StringIntegerToDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-45", "12.0");
}

TEST_F(RandomTest, StringDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(2.587964));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-45.0", "12.0");
}

TEST_F(RandomTest, StringDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(2.587964));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-45.0", "12.0");
}

TEST_F(RandomTest, StringReversedDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "6.05", "-78.686");
}

TEST_F(RandomTest, StringReversedDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "6.05", "-78.686");
}

TEST_F(RandomTest, MixedStringIntegerWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-45", 12);
}

TEST_F(RandomTest, MixedStringIntegerWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-45", 12);
}

TEST_F(RandomTest, MixedNumberWithStringInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, -45, "12");
}

TEST_F(RandomTest, MixedNumberWithStringInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, -45, "12");
}

TEST_F(RandomTest, MixedStringDoubleWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-45.0", 12);
}

TEST_F(RandomTest, MixedStringDoubleWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-45.0", 12);
}

TEST_F(RandomTest, MixedNumberWithStringDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, -45, "12.0");
}

TEST_F(RandomTest, MixedNumberWithStringDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, -45, "12.0");
}

TEST_F(RandomTest, MixedDoubleWithStringDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, 6.05, "-78.686");
}

TEST_F(RandomTest, MixedDoubleWithStringDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, 6.05, "-78.686");
}

TEST_F(RandomTest, MixedStringDoubleWithDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "6.05", -78.686);
}

TEST_F(RandomTest, MixedStringDoubleWithDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "6.05", -78.686);
}

TEST_F(RandomTest, BooleanRange)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, false, true);
}

TEST_F(RandomTest, BooleanRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, false, true);
}

TEST_F(RandomTest, BooleanWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, true, 5);
}

TEST_F(RandomTest, BooleanWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, true, 5);
}

TEST_F(RandomTest, NumberWithBoolean)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, 8, false);
}

TEST_F(RandomTest, NumberWithBoolean_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, 8, false);
}

TEST_F(RandomTest, NaNWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 5)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "NaN", 5);
}

TEST_F(RandomTest, NaNWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 5)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "NaN", 5);
}

TEST_F(RandomTest, NumberWithNaN)
{
    EXPECT_CALL(m_utils.rng(), randint(5, 0)).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, 5, "NaN");
}

TEST_F(RandomTest, NumberWithNaN_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(5, 0)).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, 5, "NaN");
}

TEST_F(RandomTest, NaNWithNaN)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 0)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "NaN", "NaN");
}

TEST_F(RandomTest, NaNWithNaN_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 0)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "NaN", "NaN");
}

TEST_F(RandomTest, InfinityWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "Infinity", 2);
}

TEST_F(RandomTest, InfinityWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "Infinity", 2);
}

TEST_F(RandomTest, NumberWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, -8, "Infinity");
}

TEST_F(RandomTest, NumberWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, -8, "Infinity");
}

TEST_F(RandomTest, NegativeInfinityWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-Infinity", -2);
}

TEST_F(RandomTest, NegativeInfinityWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-Infinity", -2);
}

TEST_F(RandomTest, NumberWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, 8, "-Infinity");
}

TEST_F(RandomTest, NumberWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, 8, "-Infinity");
}

TEST_F(RandomTest, InfinityWithDouble)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "Infinity", 2.5);
}

TEST_F(RandomTest, InfinityWithDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "Infinity", 2.5);
}

TEST_F(RandomTest, DoubleWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, -8.09, "Infinity");
}

TEST_F(RandomTest, DoubleWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, -8.09, "Infinity");
}

TEST_F(RandomTest, NegativeInfinityWithDouble)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-Infinity", -2.5);
}

TEST_F(RandomTest, NegativeInfinityWithDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-Infinity", -2.5);
}

TEST_F(RandomTest, DoubleWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, 8.09, "-Infinity");
}

TEST_F(RandomTest, DoubleWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, 8.09, "-Infinity");
}

TEST_F(RandomTest, InfinityWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "Infinity", "Infinity");
}

TEST_F(RandomTest, InfinityWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "Infinity", "Infinity");
}

TEST_F(RandomTest, NegativeInfinityWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-Infinity", "-Infinity");
}

TEST_F(RandomTest, NegativeInfinityWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-Infinity", "-Infinity");
}

TEST_F(RandomTest, InfinityWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "Infinity", "-Infinity");
}

TEST_F(RandomTest, InfinityWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "Infinity", "-Infinity");
}

TEST_F(RandomTest, NegativeInfinityWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, false, "-Infinity", "Infinity");
}

TEST_F(RandomTest, NegativeInfinityWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Random, true, "-Infinity", "Infinity");
}
