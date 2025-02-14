#include <scratchcpp/stringptr.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(StringPtrTest, DefaultConstructor)
{
    StringPtr str;
    ASSERT_EQ(str.data, nullptr);
    ASSERT_EQ(str.size, 0);
    ASSERT_EQ(str.allocatedSize, 0);
}

TEST(StringPtrTest, StringConstructor)
{
    StringPtr str("test");
    ASSERT_EQ(str.size, 4);
    ASSERT_GE(str.allocatedSize, 5);
    ASSERT_TRUE(str.data);

    ASSERT_EQ(str.data[0], u't');
    ASSERT_EQ(str.data[1], u'e');
    ASSERT_EQ(str.data[2], u's');
    ASSERT_EQ(str.data[3], u't');
    ASSERT_EQ(str.data[4], u'\0');
}
