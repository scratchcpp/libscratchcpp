#include <scratchcpp/string_pool.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/stringptr.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(StringPoolTest, NewAndFree)
{
    StringPtr *str1 = string_pool_new();
    ASSERT_TRUE(str1);
    string_assign_cstring(str1, "test");
    ASSERT_EQ(str1->size, 4);

    StringPtr *str2 = string_pool_new();
    ASSERT_TRUE(str2);
    string_assign_cstring(str2, "Lorem ipsum dolor sit amet");
    ASSERT_EQ(str2->size, 26);

    string_pool_free(str1);
    string_pool_free(str2);

    StringPtr *str3 = string_pool_new();
    ASSERT_TRUE(str3);
    string_assign_cstring(str3, "Hello world");
    ASSERT_EQ(str3->size, 11);

    // str3 will be deallocated later
}
