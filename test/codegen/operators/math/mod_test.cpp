#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class ModTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(ModTest, FourModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 4, 3);
}

TEST_F(ModTest, FourModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 4, 3);
}

TEST_F(ModTest, ThreeModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 3, 3);
}

TEST_F(ModTest, ThreeModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 3, 3);
}

TEST_F(ModTest, TwoModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 2, 3);
}

TEST_F(ModTest, TwoModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 2, 3);
}

TEST_F(ModTest, OneModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 1, 3);
}

TEST_F(ModTest, OneModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 1, 3);
}

TEST_F(ModTest, ZeroModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 0, 3);
}

TEST_F(ModTest, ZeroModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 0, 3);
}

TEST_F(ModTest, NegativeOneModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -1, 3);
}

TEST_F(ModTest, NegativeOneModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -1, 3);
}

TEST_F(ModTest, NegativeTwoModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -2, 3);
}

TEST_F(ModTest, NegativeTwoModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -2, 3);
}

TEST_F(ModTest, NegativeThreeModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -3, 3);
}

TEST_F(ModTest, NegativeThreeModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -3, 3);
}

TEST_F(ModTest, NegativeFourModThree)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -4, 3);
}

TEST_F(ModTest, NegativeFourModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -4, 3);
}

TEST_F(ModTest, DecimalModTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 4.75, 2);
}

TEST_F(ModTest, DecimalModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 4.75, 2);
}

TEST_F(ModTest, NegativeDecimalModTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -4.75, 2);
}

TEST_F(ModTest, NegativeDecimalModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -4.75, 2);
}

TEST_F(ModTest, NegativeDecimalModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -4.75, -2);
}

TEST_F(ModTest, NegativeDecimalModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -4.75, -2);
}

TEST_F(ModTest, DecimalModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 4.75, -2);
}

TEST_F(ModTest, DecimalModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 4.75, -2);
}

TEST_F(ModTest, FiveModZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 5, 0);
}

TEST_F(ModTest, FiveModZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 5, 0);
}

TEST_F(ModTest, NegativeFiveModZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -5, 0);
}

TEST_F(ModTest, NegativeFiveModZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -5, 0);
}

TEST_F(ModTest, PositiveDecimalModZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 5.8, 0);
}

TEST_F(ModTest, PositiveDecimalModZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 5.8, 0);
}

TEST_F(ModTest, NegativeDecimalModZero)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -5.8, 0);
}

TEST_F(ModTest, NegativeDecimalModZero_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -5.8, 0);
}

TEST_F(ModTest, NegativeDecimalModInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -2.5, "Infinity");
}

TEST_F(ModTest, NegativeDecimalModInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -2.5, "Infinity");
}

TEST_F(ModTest, NegativeDecimalModNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -1.2, "-Infinity");
}

TEST_F(ModTest, NegativeDecimalModNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -1.2, "-Infinity");
}

TEST_F(ModTest, PositiveDecimalModInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 2.5, "Infinity");
}

TEST_F(ModTest, PositiveDecimalModInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 2.5, "Infinity");
}

TEST_F(ModTest, PositiveDecimalModNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 1.2, "-Infinity");
}

TEST_F(ModTest, PositiveDecimalModNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 1.2, "-Infinity");
}

TEST_F(ModTest, InfinityModTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, "Infinity", 2);
}

TEST_F(ModTest, InfinityModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, "Infinity", 2);
}

TEST_F(ModTest, NegativeInfinityModTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, "-Infinity", 2);
}

TEST_F(ModTest, NegativeInfinityModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, "-Infinity", 2);
}

TEST_F(ModTest, InfinityModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, "Infinity", -2);
}

TEST_F(ModTest, InfinityModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, "Infinity", -2);
}

TEST_F(ModTest, NegativeInfinityModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, "-Infinity", -2);
}

TEST_F(ModTest, NegativeInfinityModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, "-Infinity", -2);
}

TEST_F(ModTest, ThreeModNaN)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, 3, "NaN");
}

TEST_F(ModTest, ThreeModNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, 3, "NaN");
}

TEST_F(ModTest, NegativeThreeModNaN)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, -3, "NaN");
}

TEST_F(ModTest, NegativeThreeModNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, -3, "NaN");
}

TEST_F(ModTest, NaNModFive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, "NaN", 5);
}

TEST_F(ModTest, NaNModFive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, "NaN", 5);
}

TEST_F(ModTest, NaNModNegativeFive)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, false, "NaN", -5);
}

TEST_F(ModTest, NaNModNegativeFive_Const)
{
    ASSERT_NUM_OP2(m_utils, CodeGenTestUtils::OpType::Mod, true, "NaN", -5);
}
