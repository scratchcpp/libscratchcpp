#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(StringFunctionsTest, AssignAlloc)
{
    StringPtr str1;
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

TEST(StringFunctionsTest, CompareCaseSensitive)
{
    StringPtr str1, str2;

    string_assign_cstring(&str1, "abcd");
    string_assign_cstring(&str2, "abcd");
    ASSERT_EQ(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "5.25");
    string_assign_cstring(&str2, "5.26");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "aBcd");
    string_assign_cstring(&str2, "abCd");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "olleH");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "olleH");
    string_assign_cstring(&str2, "Hello");
    ASSERT_GT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "AbCdEfGh");
    string_assign_cstring(&str2, "EfGhIjKl");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "EfGhIjKl");
    string_assign_cstring(&str2, "AbCdEfGh");
    ASSERT_GT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Lorem ipsum");
    string_assign_cstring(&str2, "dolor sit amet");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "dolor sit amet");
    string_assign_cstring(&str2, "Lorem ipsum");
    ASSERT_GT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "Hello world");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello world");
    string_assign_cstring(&str2, "Hello");
    ASSERT_GT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "Hello world");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Infinity");
    string_assign_cstring(&str2, "NaN");
    ASSERT_LT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "ábčď");
    string_assign_cstring(&str2, "ábčď");
    ASSERT_EQ(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "ábčď");
    string_assign_cstring(&str2, "ábČď");
    ASSERT_GT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "test đ");
    string_assign_cstring(&str2, "test Đ");
    ASSERT_GT(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "💀");
    string_assign_cstring(&str2, "💀");
    ASSERT_EQ(string_compare_case_sensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_sensitive(str1.data, str1.size, str2.data, str2.size), 0);
}

TEST(StringFunctionsTest, CompareCaseInsensitive)
{
    StringPtr str1, str2;

    string_assign_cstring(&str1, "abcd");
    string_assign_cstring(&str2, "abcd");
    ASSERT_EQ(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "5.25");
    string_assign_cstring(&str2, "5.26");
    ASSERT_LT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "aBcd");
    string_assign_cstring(&str2, "abCd");
    ASSERT_EQ(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "olleH");
    ASSERT_LT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "olleH");
    string_assign_cstring(&str2, "Hello");
    ASSERT_GT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "AbCdEfGh");
    string_assign_cstring(&str2, "EfGhIjKl");
    ASSERT_LT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "EfGhIjKl");
    string_assign_cstring(&str2, "AbCdEfGh");
    ASSERT_GT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Lorem ipsum");
    string_assign_cstring(&str2, "dolor sit amet");
    ASSERT_GT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "dolor sit amet");
    string_assign_cstring(&str2, "Lorem ipsum");
    ASSERT_LT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "Hello world");
    ASSERT_LT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello world");
    string_assign_cstring(&str2, "Hello");
    ASSERT_GT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_GT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "Hello world");
    ASSERT_LT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "Infinity");
    string_assign_cstring(&str2, "NaN");
    ASSERT_LT(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_LT(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "ábčď");
    string_assign_cstring(&str2, "ábčď");
    ASSERT_EQ(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "ábčď");
    string_assign_cstring(&str2, "ábČď");
    ASSERT_EQ(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "test đ");
    string_assign_cstring(&str2, "test Đ");
    ASSERT_EQ(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);

    string_assign_cstring(&str1, "💀");
    string_assign_cstring(&str2, "💀");
    ASSERT_EQ(string_compare_case_insensitive(&str1, &str2), 0);
    ASSERT_EQ(string_compare_raw_case_insensitive(str1.data, str1.size, str2.data, str2.size), 0);
}

TEST(StringFunctionsTest, ContainsCaseSensitive)
{
    StringPtr str, substr;

    string_assign_cstring(&str, "abc");
    string_assign_cstring(&substr, "a");
    ASSERT_TRUE(string_contains_case_sensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "abc");
    string_assign_cstring(&substr, "e");
    ASSERT_FALSE(string_contains_case_sensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "Hello world");
    string_assign_cstring(&substr, "ello");
    ASSERT_TRUE(string_contains_case_sensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "Hello world");
    string_assign_cstring(&substr, "eLlo");
    ASSERT_FALSE(string_contains_case_sensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "test ste");
    string_assign_cstring(&substr, "ste");
    ASSERT_TRUE(string_contains_case_sensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "Hello world");
    string_assign_cstring(&substr, "olld");
    ASSERT_FALSE(string_contains_case_sensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "á");
    ASSERT_TRUE(string_contains_case_sensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "bČ");
    ASSERT_TRUE(string_contains_case_sensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "bč");
    ASSERT_FALSE(string_contains_case_sensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "ďá");
    ASSERT_FALSE(string_contains_case_sensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "Lorem💀ipsum");
    string_assign_cstring(&substr, "m💀");
    ASSERT_TRUE(string_contains_case_sensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_sensitive(str.data, substr.data));

    string_assign_cstring(&str, "Lorem ipsum");
    string_assign_cstring(&substr, "💀");
    ASSERT_FALSE(string_contains_case_sensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_sensitive(str.data, substr.data));
}

TEST(StringFunctionsTest, ContainsCaseInsensitive)
{
    StringPtr str, substr;

    string_assign_cstring(&str, "abc");
    string_assign_cstring(&substr, "a");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "abc");
    string_assign_cstring(&substr, "e");
    ASSERT_FALSE(string_contains_case_insensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "Hello world");
    string_assign_cstring(&substr, "ello");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "Hello world");
    string_assign_cstring(&substr, "eLlo");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "test ste");
    string_assign_cstring(&substr, "ste");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "Hello world");
    string_assign_cstring(&substr, "olld");
    ASSERT_FALSE(string_contains_case_insensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "á");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "bČ");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "bč");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "ábČ");
    string_assign_cstring(&substr, "ďá");
    ASSERT_FALSE(string_contains_case_insensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "Lorem💀ipsum");
    string_assign_cstring(&substr, "m💀");
    ASSERT_TRUE(string_contains_case_insensitive(&str, &substr));
    ASSERT_TRUE(string_contains_raw_case_insensitive(str.data, substr.data));

    string_assign_cstring(&str, "Lorem ipsum");
    string_assign_cstring(&substr, "💀");
    ASSERT_FALSE(string_contains_case_insensitive(&str, &substr));
    ASSERT_FALSE(string_contains_raw_case_insensitive(str.data, substr.data));
}

TEST(StringFunctionsTest, EqualCaseSensitive)
{
    StringPtr str1, str2;

    string_assign_cstring(&str1, "abcd");
    string_assign_cstring(&str2, "abcd");
    ASSERT_TRUE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_TRUE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "5.25");
    string_assign_cstring(&str2, "5.26");
    ASSERT_FALSE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "aBcd");
    string_assign_cstring(&str2, "abCd");
    ASSERT_FALSE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "olleH");
    ASSERT_FALSE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "Lorem ipsum");
    string_assign_cstring(&str2, "dolor sit amet");
    ASSERT_FALSE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "ábčď");
    string_assign_cstring(&str2, "ábČď");
    ASSERT_FALSE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "test đ");
    string_assign_cstring(&str2, "test Đ");
    ASSERT_FALSE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "💀");
    string_assign_cstring(&str2, "💀");
    ASSERT_TRUE(strings_equal_case_sensitive(&str1, &str2));
    ASSERT_TRUE(raw_strings_equal_case_sensitive(str1.data, str1.size, str2.data, str2.size));
}

TEST(StringFunctionsTest, EqualCaseInsensitive)
{
    StringPtr str1, str2;

    string_assign_cstring(&str1, "abcd");
    string_assign_cstring(&str2, "abcd");
    ASSERT_TRUE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_TRUE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "5.25");
    string_assign_cstring(&str2, "5.26");
    ASSERT_FALSE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "aBcd");
    string_assign_cstring(&str2, "abCd");
    ASSERT_TRUE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_TRUE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "Hello");
    string_assign_cstring(&str2, "olleH");
    ASSERT_FALSE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "Lorem ipsum");
    string_assign_cstring(&str2, "dolor sit amet");
    ASSERT_FALSE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_FALSE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "ábčď");
    string_assign_cstring(&str2, "ábČď");
    ASSERT_TRUE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_TRUE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "test đ");
    string_assign_cstring(&str2, "test Đ");
    ASSERT_TRUE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_TRUE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));

    string_assign_cstring(&str1, "💀");
    string_assign_cstring(&str2, "💀");
    ASSERT_TRUE(strings_equal_case_insensitive(&str1, &str2));
    ASSERT_TRUE(raw_strings_equal_case_insensitive(str1.data, str1.size, str2.data, str2.size));
}
