#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(StringFunctionsTest, AssignAlloc)
{
    StringPtr str1;
    ASSERT_FALSE(str1.data);
    ASSERT_EQ(str1.size, 0);
    ASSERT_EQ(str1.allocatedSize, 0);

    string_assign_cstring(&str1, "test");
    ASSERT_EQ(str1.size, 4);
    ASSERT_GE(str1.allocatedSize, 5);
    ASSERT_TRUE(str1.data);
    ASSERT_EQ(str1.data[0], u't');
    ASSERT_EQ(str1.data[1], u'e');
    ASSERT_EQ(str1.data[2], u's');
    ASSERT_EQ(str1.data[3], u't');
    ASSERT_EQ(str1.data[4], u'\0');

    string_assign_cstring(&str1, "Hello world");
    ASSERT_EQ(str1.size, 11);
    ASSERT_GE(str1.allocatedSize, 12);
    ASSERT_TRUE(str1.data);
    ASSERT_EQ(str1.data[0], u'H');
    ASSERT_EQ(str1.data[1], u'e');
    ASSERT_EQ(str1.data[2], u'l');
    ASSERT_EQ(str1.data[3], u'l');
    ASSERT_EQ(str1.data[4], u'o');
    ASSERT_EQ(str1.data[5], u' ');
    ASSERT_EQ(str1.data[6], u'w');
    ASSERT_EQ(str1.data[7], u'o');
    ASSERT_EQ(str1.data[8], u'r');
    ASSERT_EQ(str1.data[9], u'l');
    ASSERT_EQ(str1.data[10], u'd');
    ASSERT_EQ(str1.data[11], u'\0');

    StringPtr str2;
    string_alloc(&str2, 8);
    ASSERT_GE(str2.allocatedSize, 9);
    ASSERT_TRUE(str2.data);
    str2.data[0] = u'a';
    str2.data[1] = u'b';
    str2.data[2] = u'c';
    str2.data[3] = u'd';
    str2.data[4] = u'e';
    str2.data[5] = u'f';
    str2.data[6] = u'g';
    str2.data[7] = u'h';
    str2.data[8] = u'\0';
    str2.size = 8;
    ASSERT_EQ(str2.data[0], u'a');
    ASSERT_EQ(str2.data[1], u'b');
    ASSERT_EQ(str2.data[2], u'c');
    ASSERT_EQ(str2.data[3], u'd');
    ASSERT_EQ(str2.data[4], u'e');
    ASSERT_EQ(str2.data[5], u'f');
    ASSERT_EQ(str2.data[6], u'g');
    ASSERT_EQ(str2.data[7], u'h');
    ASSERT_EQ(str2.data[8], u'\0');

    string_assign(&str1, &str2);
    ASSERT_EQ(str2.size, str1.size);
    ASSERT_GE(str2.allocatedSize, str1.size + 1);
    ASSERT_TRUE(str2.data);
    ASSERT_NE(str2.data, str1.data);
    ASSERT_EQ(memcmp(str2.data, str1.data, (str2.size + 1) * sizeof(typeof(*str2.data))), 0);

    string_assign_cstring(&str2, "");
    ASSERT_EQ(str2.size, 0);
    ASSERT_GE(str2.allocatedSize, 1);
    ASSERT_TRUE(str2.data);
    ASSERT_EQ(str2.data[0], u'\0');

    string_assign_cstring(&str2, "test");
    string_assign_cstring(&str2, nullptr);
    ASSERT_EQ(str2.size, 0);
    ASSERT_GE(str2.allocatedSize, 1);
    ASSERT_TRUE(str2.data);
    ASSERT_EQ(str2.data[0], u'\0');

    string_assign(&str1, &str2);
    ASSERT_EQ(str1.size, 0);
    ASSERT_GE(str1.allocatedSize, 1);
    ASSERT_TRUE(str1.data);
    ASSERT_EQ(str1.data[0], u'\0');
}
