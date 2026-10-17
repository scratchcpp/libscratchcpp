#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class StringConcatTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(StringConcatTest, HelloWorld)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringConcat, false, "Hello ", "world").toString(), "Hello world");
}

TEST_F(StringConcatTest, HelloWorld_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringConcat, true, "Hello ", "world").toString(), "Hello world");
}

TEST_F(StringConcatTest, AbcDef)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringConcat, false, "abc", "def").toString(), "abcdef");
}

TEST_F(StringConcatTest, AbcDef_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringConcat, true, "abc", "def").toString(), "abcdef");
}

TEST_F(StringConcatTest, UnicodeStrings)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringConcat, false, "ábč", "ďéfgh").toString(), "ábčďéfgh");
}

TEST_F(StringConcatTest, UnicodeStrings_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringConcat, true, "ábč", "ďéfgh").toString(), "ábčďéfgh");
}
