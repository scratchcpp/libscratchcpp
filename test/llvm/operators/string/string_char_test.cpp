#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "llvmtestutils.h"

class LLVMStringCharTest : public testing::Test
{
    public:
        LLVMTestUtils m_utils;
};

TEST_F(LLVMStringCharTest, HelloWorldSecondChar)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, false, "Hello world", 1).toString(), "e");
}

TEST_F(LLVMStringCharTest, HelloWorldSecondChar_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, true, "Hello world", 1).toString(), "e");
}

TEST_F(LLVMStringCharTest, HelloWorldZeroIndex)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, false, "Hello world", 0).toString(), "H");
}

TEST_F(LLVMStringCharTest, HelloWorldZeroIndex_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, true, "Hello world", 0).toString(), "H");
}

TEST_F(LLVMStringCharTest, HelloWorldOutOfRange)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, false, "Hello world", 11).toString(), "");
}

TEST_F(LLVMStringCharTest, HelloWorldOutOfRange_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, true, "Hello world", 11).toString(), "");
}

TEST_F(LLVMStringCharTest, AbcThirdChar)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, false, "abc", 2).toString(), "c");
}

TEST_F(LLVMStringCharTest, AbcThirdChar_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, true, "abc", 2).toString(), "c");
}

TEST_F(LLVMStringCharTest, AbcNegativeIndex)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, false, "abc", -1).toString(), "");
}

TEST_F(LLVMStringCharTest, AbcNegativeIndex_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, true, "abc", -1).toString(), "");
}

TEST_F(LLVMStringCharTest, AbcOutOfRange)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, false, "abc", 3).toString(), "");
}

TEST_F(LLVMStringCharTest, AbcOutOfRange_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, true, "abc", 3).toString(), "");
}

TEST_F(LLVMStringCharTest, UnicodeString)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, false, "ábč", 0).toString(), "á");
}

TEST_F(LLVMStringCharTest, UnicodeString_Const)
{
    ASSERT_EQ(m_utils.getOpResult(LLVMTestUtils::OpType::StringChar, true, "ábč", 0).toString(), "á");
}
