#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMModTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMModTest, FourModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 4, 3);
}

TEST_F(LLVMModTest, FourModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 4, 3);
}

TEST_F(LLVMModTest, ThreeModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 3, 3);
}

TEST_F(LLVMModTest, ThreeModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 3, 3);
}

TEST_F(LLVMModTest, TwoModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 2, 3);
}

TEST_F(LLVMModTest, TwoModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 2, 3);
}

TEST_F(LLVMModTest, OneModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 1, 3);
}

TEST_F(LLVMModTest, OneModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 1, 3);
}

TEST_F(LLVMModTest, ZeroModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 0, 3);
}

TEST_F(LLVMModTest, ZeroModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 0, 3);
}

TEST_F(LLVMModTest, NegativeOneModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -1, 3);
}

TEST_F(LLVMModTest, NegativeOneModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -1, 3);
}

TEST_F(LLVMModTest, NegativeTwoModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -2, 3);
}

TEST_F(LLVMModTest, NegativeTwoModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -2, 3);
}

TEST_F(LLVMModTest, NegativeThreeModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -3, 3);
}

TEST_F(LLVMModTest, NegativeThreeModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -3, 3);
}

TEST_F(LLVMModTest, NegativeFourModThree)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -4, 3);
}

TEST_F(LLVMModTest, NegativeFourModThree_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -4, 3);
}

TEST_F(LLVMModTest, DecimalModTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 4.75, 2);
}

TEST_F(LLVMModTest, DecimalModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 4.75, 2);
}

TEST_F(LLVMModTest, NegativeDecimalModTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -4.75, 2);
}

TEST_F(LLVMModTest, NegativeDecimalModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -4.75, 2);
}

TEST_F(LLVMModTest, NegativeDecimalModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -4.75, -2);
}

TEST_F(LLVMModTest, NegativeDecimalModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -4.75, -2);
}

TEST_F(LLVMModTest, DecimalModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 4.75, -2);
}

TEST_F(LLVMModTest, DecimalModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 4.75, -2);
}

TEST_F(LLVMModTest, FiveModZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 5, 0);
}

TEST_F(LLVMModTest, FiveModZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 5, 0);
}

TEST_F(LLVMModTest, NegativeFiveModZero)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -5, 0);
}

TEST_F(LLVMModTest, NegativeFiveModZero_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -5, 0);
}

TEST_F(LLVMModTest, NegativeDecimalModInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -2.5, "Infinity");
}

TEST_F(LLVMModTest, NegativeDecimalModInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -2.5, "Infinity");
}

TEST_F(LLVMModTest, NegativeDecimalModNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -1.2, "-Infinity");
}

TEST_F(LLVMModTest, NegativeDecimalModNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -1.2, "-Infinity");
}

TEST_F(LLVMModTest, PositiveDecimalModInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 2.5, "Infinity");
}

TEST_F(LLVMModTest, PositiveDecimalModInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 2.5, "Infinity");
}

TEST_F(LLVMModTest, PositiveDecimalModNegativeInfinity)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 1.2, "-Infinity");
}

TEST_F(LLVMModTest, PositiveDecimalModNegativeInfinity_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 1.2, "-Infinity");
}

TEST_F(LLVMModTest, InfinityModTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, "Infinity", 2);
}

TEST_F(LLVMModTest, InfinityModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, "Infinity", 2);
}

TEST_F(LLVMModTest, NegativeInfinityModTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, "-Infinity", 2);
}

TEST_F(LLVMModTest, NegativeInfinityModTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, "-Infinity", 2);
}

TEST_F(LLVMModTest, InfinityModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, "Infinity", -2);
}

TEST_F(LLVMModTest, InfinityModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, "Infinity", -2);
}

TEST_F(LLVMModTest, NegativeInfinityModNegativeTwo)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, "-Infinity", -2);
}

TEST_F(LLVMModTest, NegativeInfinityModNegativeTwo_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, "-Infinity", -2);
}

TEST_F(LLVMModTest, ThreeModNaN)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, 3, "NaN");
}

TEST_F(LLVMModTest, ThreeModNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, 3, "NaN");
}

TEST_F(LLVMModTest, NegativeThreeModNaN)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, -3, "NaN");
}

TEST_F(LLVMModTest, NegativeThreeModNaN_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, -3, "NaN");
}

TEST_F(LLVMModTest, NaNModFive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, "NaN", 5);
}

TEST_F(LLVMModTest, NaNModFive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, "NaN", 5);
}

TEST_F(LLVMModTest, NaNModNegativeFive)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, false, "NaN", -5);
}

TEST_F(LLVMModTest, NaNModNegativeFive_Const)
{
    ASSERT_NUM_OP2(m_utils, LLVMTestUtils::OpType::Mod, true, "NaN", -5);
}
