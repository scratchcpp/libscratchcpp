#include <scratchcpp/list.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(ListTest, Constructors)
{
    List list("abc", "test list");
    ASSERT_EQ(list.id(), "abc");
    ASSERT_EQ(list.name(), "test list");
}

TEST(ListTest, Name)
{
    List list("", "");
    list.setName("test list");
    ASSERT_EQ(list.name(), "test list");
}

TEST(ListTest, IndexOf)
{
    List list("", "test list");
    list.push_back("Lorem");
    list.push_back("ipsum");
    list.push_back("dolor");
    list.push_back("sit");
    list.push_back("amet");
    list.push_back("é čľíá");

    ASSERT_EQ(list.indexOf(""), -1);
    ASSERT_EQ(list.indexOf("test"), -1);
    ASSERT_EQ(list.indexOf("Lorem"), 0);
    ASSERT_EQ(list.indexOf("ipsum"), 1);
    ASSERT_EQ(list.indexOf("iPsum"), 1);
    ASSERT_EQ(list.indexOf("dolor"), 2);
    ASSERT_EQ(list.indexOf("sit"), 3);
    ASSERT_EQ(list.indexOf("amet"), 4);
    ASSERT_EQ(list.indexOf("é čľíá"), 5);

    list.clear();
    list.push_back("");
    ASSERT_EQ(list.indexOf(""), 0);
}

TEST(ListTest, Contains)
{
    List list("", "test list");
    list.push_back("Lorem");
    list.push_back("ipsum");
    list.push_back("dolor");
    list.push_back("sit");
    list.push_back("amet");
    list.push_back("é čľíá");

    ASSERT_FALSE(list.contains(""));
    ASSERT_FALSE(list.contains("test"));
    ASSERT_TRUE(list.contains("Lorem"));
    ASSERT_TRUE(list.contains("ipsum"));
    ASSERT_TRUE(list.contains("iPsum"));
    ASSERT_TRUE(list.contains("dolor"));
    ASSERT_TRUE(list.contains("sit"));
    ASSERT_TRUE(list.contains("amet"));
    ASSERT_TRUE(list.contains("é čľíá"));

    list.clear();
    list.push_back("");
    ASSERT_TRUE(list.contains(""));
}

TEST(ListTest, RemoveAt)
{
    List list("", "test list");
    list.push_back("Lorem");
    list.push_back("ipsum");
    list.push_back("dolor");
    list.push_back("sit");
    list.push_back("amet");
    list.removeAt(1);
    ASSERT_EQ(list.toString(), "Lorem dolor sit amet");
    list.removeAt(3);
    ASSERT_EQ(list.toString(), "Lorem dolor sit");
    list.removeAt(0);
    ASSERT_EQ(list.toString(), "dolor sit");
    list.removeAt(3);
    ASSERT_EQ(list.toString(), "dolor");
    list.removeAt(0);
    ASSERT_TRUE(list.empty());
}

TEST(ListTest, Insert)
{
    List list("", "test list");
    list.insert(0, "Lorem");
    list.insert(0, "ipsum");
    list.insert(0, "dolor");
    ASSERT_EQ(list.toString(), "dolor ipsum Lorem");
    list.insert(1, "sit");
    ASSERT_EQ(list.toString(), "dolor sit ipsum Lorem");
    list.insert(2, "amet");
    ASSERT_EQ(list.toString(), "dolor sit amet ipsum Lorem");
    list.insert(5, "consectetur");
    ASSERT_EQ(list.toString(), "dolor sit amet ipsum Lorem consectetur");
    list.insert(0, "adipiscing");
    ASSERT_EQ(list.toString(), "adipiscing dolor sit amet ipsum Lorem consectetur");
}

TEST(ListTest, Replace)
{
    List list("", "test list");
    list.push_back("Lorem");
    list.push_back("ipsum");
    list.push_back("dolor");
    list.push_back("sit");
    list.push_back("amet");
    ASSERT_EQ(list.toString(), "Lorem ipsum dolor sit amet");
    list.replace(0, "test1");
    ASSERT_EQ(list.toString(), "test1 ipsum dolor sit amet");
    list.replace(4, "test2");
    ASSERT_EQ(list.toString(), "test1 ipsum dolor sit test2");
    list.replace(2, "test3");
    ASSERT_EQ(list.toString(), "test1 ipsum test3 sit test2");
}

TEST(ListTest, ToString)
{
    List list("", "test list");
    ASSERT_EQ(list.toString(), "");

    list.push_back("");
    ASSERT_EQ(list.toString(), "");

    list.push_back("");
    list.push_back("");
    ASSERT_EQ(list.toString(), "  ");

    list.clear();
    list.push_back("item1");
    list.push_back("i t e m 2");
    list.push_back("item 3");
    ASSERT_EQ(list.toString(), "item1 i t e m 2 item 3");

    list.clear();
    list.push_back(" ");
    list.push_back("a ");
    list.push_back(" b");
    list.push_back(" c ");
    ASSERT_EQ(list.toString(), "  a   b  c ");

    list.clear();
    list.push_back("áä");
    list.push_back("ľ š");
    ASSERT_EQ(list.toString(), "áä ľ š");

    list.clear();
    list.push_back(-2);
    list.push_back(5);
    list.push_back(8);
    ASSERT_EQ(list.toString(), "-2 5 8");

    list.clear();
    list.push_back(2);
    list.push_back(10);
    list.push_back(8);
    ASSERT_EQ(list.toString(), "2 10 8");

    list.clear();
    list.push_back(0);
    list.push_back(9);
    list.push_back(8);
    ASSERT_EQ(list.toString(), "098");
}

TEST(ListTest, Clone)
{
    List list("abc", "test list");
    list.push_back("Lorem");
    list.push_back("ipsum");
    list.push_back("dolor");
    list.push_back("sit");
    list.push_back("amet");

    std::shared_ptr<List> clone = list.clone();
    ASSERT_TRUE(clone);
    ASSERT_EQ(clone->id(), list.id());
    ASSERT_EQ(clone->name(), list.name());
    ASSERT_EQ(clone->size(), list.size());

    for (std::size_t i = 0; i < list.size(); i++)
        ASSERT_EQ(list[i], (*clone)[i]);
}
