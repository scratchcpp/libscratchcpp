#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMStringLengthTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMStringLengthTest, HelloWorld)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, false, "Hello world").toDouble(), 11);
}

TEST_F(LLVMStringLengthTest, HelloWorld_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, true, "Hello world").toDouble(), 11);
}

TEST_F(LLVMStringLengthTest, Abc)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, false, "abc").toDouble(), 3);
}

TEST_F(LLVMStringLengthTest, Abc_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, true, "abc").toDouble(), 3);
}

TEST_F(LLVMStringLengthTest, Abcdef)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, false, "abcdef").toDouble(), 6);
}

TEST_F(LLVMStringLengthTest, Abcdef_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, true, "abcdef").toDouble(), 6);
}

TEST_F(LLVMStringLengthTest, UnicodeString)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, false, "ábč").toDouble(), 3);
}

TEST_F(LLVMStringLengthTest, UnicodeString_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringLength, true, "ábč").toDouble(), 3);
}
