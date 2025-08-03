#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

using ::testing::Return;

class LLVMRandomTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMRandomTest, IntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, -45, 12);
}

TEST_F(LLVMRandomTest, IntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(-18));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, -45, 12);
}

TEST_F(LLVMRandomTest, DoubleAsInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, -45.0, 12.0);
}

TEST_F(LLVMRandomTest, DoubleAsInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, -45.0, 12.0);
}

TEST_F(LLVMRandomTest, DoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(12, 6.05)).WillRepeatedly(Return(3.486789));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, 12, 6.05);
}

TEST_F(LLVMRandomTest, DoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(12, 6.05)).WillRepeatedly(Return(3.486789));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, 12, 6.05);
}

TEST_F(LLVMRandomTest, NegativeDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-78.686, -45)).WillRepeatedly(Return(-59.468873));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, -78.686, -45);
}

TEST_F(LLVMRandomTest, NegativeDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-78.686, -45)).WillRepeatedly(Return(-59.468873));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, -78.686, -45);
}

TEST_F(LLVMRandomTest, ReversedDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(-28.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, 6.05, -78.686);
}

TEST_F(LLVMRandomTest, ReversedDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(-28.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, 6.05, -78.686);
}

TEST_F(LLVMRandomTest, StringIntegerRange)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-45", "12");
}

TEST_F(LLVMRandomTest, StringIntegerRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-45", "12");
}

TEST_F(LLVMRandomTest, StringDoubleToInteger)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-45.0", "12");
}

TEST_F(LLVMRandomTest, StringDoubleToInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-45.0", "12");
}

TEST_F(LLVMRandomTest, StringIntegerToDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-45", "12.0");
}

TEST_F(LLVMRandomTest, StringIntegerToDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-45", "12.0");
}

TEST_F(LLVMRandomTest, StringDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(2.587964));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-45.0", "12.0");
}

TEST_F(LLVMRandomTest, StringDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(2.587964));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-45.0", "12.0");
}

TEST_F(LLVMRandomTest, StringReversedDoubleRange)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "6.05", "-78.686");
}

TEST_F(LLVMRandomTest, StringReversedDoubleRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "6.05", "-78.686");
}

TEST_F(LLVMRandomTest, MixedStringIntegerWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-45", 12);
}

TEST_F(LLVMRandomTest, MixedStringIntegerWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-45", 12);
}

TEST_F(LLVMRandomTest, MixedNumberWithStringInteger)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, -45, "12");
}

TEST_F(LLVMRandomTest, MixedNumberWithStringInteger_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(-45, 12)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, -45, "12");
}

TEST_F(LLVMRandomTest, MixedStringDoubleWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-45.0", 12);
}

TEST_F(LLVMRandomTest, MixedStringDoubleWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(5.2));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-45.0", 12);
}

TEST_F(LLVMRandomTest, MixedNumberWithStringDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, -45, "12.0");
}

TEST_F(LLVMRandomTest, MixedNumberWithStringDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(-45, 12)).WillRepeatedly(Return(-15.5787));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, -45, "12.0");
}

TEST_F(LLVMRandomTest, MixedDoubleWithStringDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, 6.05, "-78.686");
}

TEST_F(LLVMRandomTest, MixedDoubleWithStringDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, 6.05, "-78.686");
}

TEST_F(LLVMRandomTest, MixedStringDoubleWithDouble)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "6.05", -78.686);
}

TEST_F(LLVMRandomTest, MixedStringDoubleWithDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randintDouble(6.05, -78.686)).WillRepeatedly(Return(5.648764));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "6.05", -78.686);
}

TEST_F(LLVMRandomTest, BooleanRange)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, false, true);
}

TEST_F(LLVMRandomTest, BooleanRange_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 1)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, false, true);
}

TEST_F(LLVMRandomTest, BooleanWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, true, 5);
}

TEST_F(LLVMRandomTest, BooleanWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(1, 5)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, true, 5);
}

TEST_F(LLVMRandomTest, NumberWithBoolean)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, 8, false);
}

TEST_F(LLVMRandomTest, NumberWithBoolean_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(8, 0)).WillRepeatedly(Return(1));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, 8, false);
}

TEST_F(LLVMRandomTest, NaNWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 5)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "NaN", 5);
}

TEST_F(LLVMRandomTest, NaNWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 5)).WillRepeatedly(Return(5));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "NaN", 5);
}

TEST_F(LLVMRandomTest, NumberWithNaN)
{
    EXPECT_CALL(m_utils.rng(), randint(5, 0)).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, 5, "NaN");
}

TEST_F(LLVMRandomTest, NumberWithNaN_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(5, 0)).WillRepeatedly(Return(3));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, 5, "NaN");
}

TEST_F(LLVMRandomTest, NaNWithNaN)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 0)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "NaN", "NaN");
}

TEST_F(LLVMRandomTest, NaNWithNaN_Const)
{
    EXPECT_CALL(m_utils.rng(), randint(0, 0)).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "NaN", "NaN");
}

TEST_F(LLVMRandomTest, InfinityWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "Infinity", 2);
}

TEST_F(LLVMRandomTest, InfinityWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "Infinity", 2);
}

TEST_F(LLVMRandomTest, NumberWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, -8, "Infinity");
}

TEST_F(LLVMRandomTest, NumberWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, -8, "Infinity");
}

TEST_F(LLVMRandomTest, NegativeInfinityWithNumber)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-Infinity", -2);
}

TEST_F(LLVMRandomTest, NegativeInfinityWithNumber_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-Infinity", -2);
}

TEST_F(LLVMRandomTest, NumberWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, 8, "-Infinity");
}

TEST_F(LLVMRandomTest, NumberWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, 8, "-Infinity");
}

TEST_F(LLVMRandomTest, InfinityWithDouble)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "Infinity", 2.5);
}

TEST_F(LLVMRandomTest, InfinityWithDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "Infinity", 2.5);
}

TEST_F(LLVMRandomTest, DoubleWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, -8.09, "Infinity");
}

TEST_F(LLVMRandomTest, DoubleWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, -8.09, "Infinity");
}

TEST_F(LLVMRandomTest, NegativeInfinityWithDouble)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-Infinity", -2.5);
}

TEST_F(LLVMRandomTest, NegativeInfinityWithDouble_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-Infinity", -2.5);
}

TEST_F(LLVMRandomTest, DoubleWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, 8.09, "-Infinity");
}

TEST_F(LLVMRandomTest, DoubleWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, 8.09, "-Infinity");
}

TEST_F(LLVMRandomTest, InfinityWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "Infinity", "Infinity");
}

TEST_F(LLVMRandomTest, InfinityWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "Infinity", "Infinity");
}

TEST_F(LLVMRandomTest, NegativeInfinityWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-Infinity", "-Infinity");
}

TEST_F(LLVMRandomTest, NegativeInfinityWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-Infinity", "-Infinity");
}

TEST_F(LLVMRandomTest, InfinityWithNegativeInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "Infinity", "-Infinity");
}

TEST_F(LLVMRandomTest, InfinityWithNegativeInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "Infinity", "-Infinity");
}

TEST_F(LLVMRandomTest, NegativeInfinityWithInfinity)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, false, "-Infinity", "Infinity");
}

TEST_F(LLVMRandomTest, NegativeInfinityWithInfinity_Const)
{
    EXPECT_CALL(m_utils.rng(), randint).WillRepeatedly(Return(0));
    EXPECT_CALL(m_utils.rng(), randintDouble).WillRepeatedly(Return(0));
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Random, true, "-Infinity", "Infinity");
}
