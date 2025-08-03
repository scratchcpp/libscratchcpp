#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMStringConcatTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMStringConcatTest, HelloWorld)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringConcat, false, "Hello ", "world").toString(), "Hello world");
}

TEST_F(LLVMStringConcatTest, HelloWorld_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringConcat, true, "Hello ", "world").toString(), "Hello world");
}

TEST_F(LLVMStringConcatTest, AbcDef)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringConcat, false, "abc", "def").toString(), "abcdef");
}

TEST_F(LLVMStringConcatTest, AbcDef_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringConcat, true, "abc", "def").toString(), "abcdef");
}

TEST_F(LLVMStringConcatTest, UnicodeStrings)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringConcat, false, "ábč", "ďéfgh").toString(), "ábčďéfgh");
}

TEST_F(LLVMStringConcatTest, UnicodeStrings_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringConcat, true, "ábč", "ďéfgh").toString(), "ábčďéfgh");
}
