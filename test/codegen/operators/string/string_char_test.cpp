#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class StringCharTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(StringCharTest, HelloWorldSecondChar)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, false, "Hello world", 1).toString(), "e");
}

TEST_F(StringCharTest, HelloWorldSecondChar_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, true, "Hello world", 1).toString(), "e");
}

TEST_F(StringCharTest, HelloWorldZeroIndex)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, false, "Hello world", 0).toString(), "H");
}

TEST_F(StringCharTest, HelloWorldZeroIndex_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, true, "Hello world", 0).toString(), "H");
}

TEST_F(StringCharTest, HelloWorldOutOfRange)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, false, "Hello world", 11).toString(), "");
}

TEST_F(StringCharTest, HelloWorldOutOfRange_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, true, "Hello world", 11).toString(), "");
}

TEST_F(StringCharTest, AbcThirdChar)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, false, "abc", 2).toString(), "c");
}

TEST_F(StringCharTest, AbcThirdChar_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, true, "abc", 2).toString(), "c");
}

TEST_F(StringCharTest, AbcNegativeIndex)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, false, "abc", -1).toString(), "");
}

TEST_F(StringCharTest, AbcNegativeIndex_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, true, "abc", -1).toString(), "");
}

TEST_F(StringCharTest, AbcOutOfRange)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, false, "abc", 3).toString(), "");
}

TEST_F(StringCharTest, AbcOutOfRange_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, true, "abc", 3).toString(), "");
}

TEST_F(StringCharTest, UnicodeString)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, false, "ábč", 0).toString(), "á");
}

TEST_F(StringCharTest, UnicodeString_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringChar, true, "ábč", 0).toString(), "á");
}
