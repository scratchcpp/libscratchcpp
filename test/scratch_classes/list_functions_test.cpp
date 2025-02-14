#include <scratchcpp/list.h>
#include <scratch/list_functions.h>
#include <utf8.h>

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

TEST(ListFunctionsTest, AppendEmpty)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");

    ValueData *v = list_append_empty(&list);
    value_init(v);
    value_assign_double(v, 5);
    ASSERT_EQ(list.toString(), "Lorem ipsum 5");

    v = list_append_empty(&list);
    value_init(v);
    value_assign_string(v, "test");
    ASSERT_EQ(list.toString(), "Lorem ipsum 5 test");
}

TEST(ListFunctionsTest, InsertEmpty)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");

    ValueData *v = list_insert_empty(&list, 0);
    value_init(v);
    value_assign_double(v, 5);
    ASSERT_EQ(list.toString(), "5 Lorem ipsum");

    v = list_insert_empty(&list, 2);
    value_init(v);
    value_assign_string(v, "test");
    ASSERT_EQ(list.toString(), "5 Lorem test ipsum");
}

TEST(ListFunctionsTest, GetItem)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    ASSERT_EQ(list_get_item(&list, 0), &list[0]);
    ASSERT_EQ(list_get_item(&list, 1), &list[1]);
    ASSERT_EQ(list_get_item(&list, 2), &list[2]);
    ASSERT_EQ(list_get_item(&list, 3), &list[3]);
    ASSERT_EQ(list_get_item(&list, 4), &list[4]);
}

TEST(ListFunctionsTest, Data)
{
    List list("", "");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    ValueData *data = list_data(&list);
    ASSERT_EQ(data, list.data());
}

TEST(ListFunctionsTest, SizePtr)
{
    List list("", "");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    size_t *sizePtr = list_size_ptr(&list);
    ASSERT_EQ(sizePtr, list.sizePtr());
}

TEST(ListFunctionsTest, AllocSizePtr)
{
    List list("", "");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    const size_t *sizePtr = list_alloc_size_ptr(&list);
    ASSERT_EQ(sizePtr, list.allocatedSizePtr());
}

TEST(ListFunctionsTest, Size)
{
    {
        List list("", "");
        list.append("Lorem");
        list.append("ipsum");
        list.append("dolor");
        list.append("sit");
        list.append("amet");

        ASSERT_EQ(list_size(&list), 5);
    }

    {
        List list("", "");
        list.append("1");
        list.append("2");

        ASSERT_EQ(list_size(&list), 2);
    }
}

TEST(ListFunctionsTest, ToString)
{
    {
        List list("", "test list");
        list.append("Lorem");
        list.append("ipsum");
        list.append("dolor");
        list.append("sit");
        list.append("amet");

        StringPtr *str = list_to_string(&list);
        ASSERT_EQ(utf8::utf16to8(std::u16string(str->data)), "Lorem ipsum dolor sit amet");
    }

    {
        List list("", "test list");
        list.append(1);
        list.append(2);
        list.append(3);

        StringPtr *str = list_to_string(&list);
        ASSERT_EQ(utf8::utf16to8(std::u16string(str->data)), "123");
    }
}
