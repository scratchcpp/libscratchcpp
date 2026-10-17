#include <gtest/gtest.h>
#include <scratchcpp/value.h>

#include "codegentestutils.h"

class StringLengthTest : public testing::Test
{
    public:
        CodeGenTestUtils m_utils;
};

TEST_F(StringLengthTest, HelloWorld)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, false, "Hello world").toDouble(), 11);
}

TEST_F(StringLengthTest, HelloWorld_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, true, "Hello world").toDouble(), 11);
}

TEST_F(StringLengthTest, Abc)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, false, "abc").toDouble(), 3);
}

TEST_F(StringLengthTest, Abc_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, true, "abc").toDouble(), 3);
}

TEST_F(StringLengthTest, Abcdef)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, false, "abcdef").toDouble(), 6);
}

TEST_F(StringLengthTest, Abcdef_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, true, "abcdef").toDouble(), 6);
}

TEST_F(StringLengthTest, UnicodeString)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, false, "ábč").toDouble(), 3);
}

TEST_F(StringLengthTest, UnicodeString_Const)
{
    ASSERT_EQ(m_utils.getOpResult(CodeGenTestUtils::OpType::StringLength, true, "ábč").toDouble(), 3);
}
