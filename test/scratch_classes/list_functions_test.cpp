#include <scratchcpp/list.h>
#include <scratch/list_functions.h>

#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(ListFunctionsTest, Clear)
{
    List list("", "");
    list.append(1);
    list.append(2);
    list.append(3);

    list_clear(&list);
}

TEST(ListFunctionsTest, Remove)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    list_remove(&list, 1);
    ASSERT_EQ(list.toString(), "Lorem dolor sit amet");
    list_remove(&list, 3);
    ASSERT_EQ(list.toString(), "Lorem dolor sit");
    list_remove(&list, 0);
    ASSERT_EQ(list.toString(), "dolor sit");
    list_remove(&list, 1);
    ASSERT_EQ(list.toString(), "dolor");
    list_remove(&list, 0);
    ASSERT_TRUE(list.empty());
}
