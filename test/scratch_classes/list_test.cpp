#include <scratchcpp/list.h>
#include <scratchcpp/target.h>
#include <scratchcpp/monitor.h>

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

TEST(ListTest, Target)
{
    List list("", "");
    ASSERT_EQ(list.target(), nullptr);

    Target target;
    list.setTarget(&target);
    ASSERT_EQ(list.target(), &target);
}

TEST(ListTest, Monitor)
{
    List list("", "");
    ASSERT_EQ(list.monitor(), nullptr);

    Monitor monitor("", "");
    list.setMonitor(&monitor);
    ASSERT_EQ(list.monitor(), &monitor);
}

TEST(ListTest, Data)
{
    List list("", "");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    ValueData *data = list.data();
    ASSERT_EQ(&data[0], &list[0]);
    ASSERT_EQ(&data[1], &list[1]);
    ASSERT_EQ(&data[2], &list[2]);
    ASSERT_EQ(&data[3], &list[3]);
    ASSERT_EQ(&data[4], &list[4]);
}

TEST(ListTest, SizePtr)
{
    List list("", "test list");
    ASSERT_TRUE(list.sizePtr());
    ASSERT_EQ(*list.sizePtr(), 0);
    const size_t *ptr = list.sizePtr();
    ASSERT_TRUE(list.empty());

    list.append("Lorem");
    list.append("ipsum");
    ASSERT_EQ(*list.sizePtr(), 2);
    ASSERT_FALSE(list.empty());

    list.append("dolor");
    ASSERT_EQ(*list.sizePtr(), 3);
    ASSERT_EQ(list.sizePtr(), ptr);
    ASSERT_FALSE(list.empty());

    list.removeAt(0);
    ASSERT_EQ(*list.sizePtr(), 2);
    ASSERT_FALSE(list.empty());

    *list.sizePtr() = 100;
    ASSERT_EQ(*list.sizePtr(), 100);
    ASSERT_EQ(list.size(), 100);
}

TEST(ListTest, AllocatedSizePtr)
{
    List list("", "test list");
    ASSERT_TRUE(list.allocatedSizePtr());
    ASSERT_EQ(*list.allocatedSizePtr(), 0);
    const size_t *ptr = list.allocatedSizePtr();

    list.append("Lorem");
    list.append("ipsum");

    ASSERT_GT(*list.allocatedSizePtr(), 0);
    ASSERT_EQ(list.allocatedSizePtr(), ptr);
    ASSERT_NE(list.allocatedSizePtr(), list.sizePtr());
}

TEST(ListTest, Size)
{
    List list("", "test list");
    ASSERT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());

    list.append("Lorem");
    list.append("ipsum");
    ASSERT_EQ(list.size(), 2);
    ASSERT_FALSE(list.empty());

    list.append("dolor");
    ASSERT_EQ(list.size(), 3);
    ASSERT_FALSE(list.empty());

    list.removeAt(0);
    ASSERT_EQ(list.size(), 2);
    ASSERT_FALSE(list.empty());
}

TEST(ListTest, IndexOf)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");
    list.append("é čľíá");

    ASSERT_EQ(list.indexOf(""), -1);
    ASSERT_EQ(list.indexOf("test"), -1);
    ASSERT_EQ(list.indexOf("Lorem"), 0);
    ASSERT_EQ(list.indexOf("ipsum"), 1);
    ASSERT_EQ(list.indexOf(Value("iPsum").data()), 1);
    ASSERT_EQ(list.indexOf(Value("dolor").data()), 2);
    ASSERT_EQ(list.indexOf("sit"), 3);
    ASSERT_EQ(list.indexOf("amet"), 4);
    ASSERT_EQ(list.indexOf("é čľíá"), 5);

    list.clear();
    list.append("");
    ASSERT_EQ(list.indexOf(""), 0);
}

TEST(ListTest, Contains)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");
    list.append("é čľíá");

    ASSERT_FALSE(list.contains(""));
    ASSERT_FALSE(list.contains(Value("test").data()));
    ASSERT_FALSE(list.contains("test"));
    ASSERT_TRUE(list.contains("Lorem"));
    ASSERT_TRUE(list.contains("ipsum"));
    ASSERT_TRUE(list.contains("iPsum"));
    ASSERT_TRUE(list.contains("dolor"));
    ASSERT_TRUE(list.contains(Value("sit").data()));
    ASSERT_TRUE(list.contains("amet"));
    ASSERT_TRUE(list.contains("é čľíá"));

    list.clear();
    list.append("");
    ASSERT_TRUE(list.contains(""));
}

TEST(ListTest, Clear)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");
    list.append("é čľíá");

    list.clear();
    ASSERT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());
}

TEST(ListTest, Append)
{
    List list("", "test list");
    list.append("Lorem");
    list.append(Value("ipsum").data());
    list.append("dolor");
    list.append(Value("sit").data());
    list.append("amet");
    ASSERT_EQ(list.toString(), "Lorem ipsum dolor sit amet");

    list.append("consectetur");
    ASSERT_EQ(list.toString(), "Lorem ipsum dolor sit amet consectetur");

    list.clear();
    list.append("adipiscing");
    ASSERT_EQ(list.toString(), "adipiscing");
}

TEST(ListTest, RemoveAt)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");
    list.removeAt(1);
    ASSERT_EQ(list.toString(), "Lorem dolor sit amet");
    list.removeAt(3);
    ASSERT_EQ(list.toString(), "Lorem dolor sit");
    list.removeAt(0);
    ASSERT_EQ(list.toString(), "dolor sit");
    list.removeAt(1);
    ASSERT_EQ(list.toString(), "dolor");
    list.removeAt(0);
    ASSERT_TRUE(list.empty());
}

TEST(ListTest, Insert)
{
    List list("", "test list");
    list.insert(0, Value("Lorem").data());
    list.insert(0, "ipsum");
    list.insert(0, "dolor");
    ASSERT_EQ(list.toString(), "dolor ipsum Lorem");
    list.insert(1, "sit");
    ASSERT_EQ(list.toString(), "dolor sit ipsum Lorem");
    list.insert(2, Value("amet").data());
    ASSERT_EQ(list.toString(), "dolor sit amet ipsum Lorem");
    list.insert(5, "consectetur");
    ASSERT_EQ(list.toString(), "dolor sit amet ipsum Lorem consectetur");
    list.insert(0, "adipiscing");
    ASSERT_EQ(list.toString(), "adipiscing dolor sit amet ipsum Lorem consectetur");
}

TEST(ListTest, Replace)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");
    ASSERT_EQ(list.toString(), "Lorem ipsum dolor sit amet");
    list.replace(0, "test1");
    ASSERT_EQ(list.toString(), "test1 ipsum dolor sit amet");
    list.replace(4, Value("test2").data());
    ASSERT_EQ(list.toString(), "test1 ipsum dolor sit test2");
    list.replace(2, Value("test3").data());
    ASSERT_EQ(list.toString(), "test1 ipsum test3 sit test2");
}

TEST(ListTest, ArrayIndexOperator)
{
    List list("", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    ASSERT_EQ(Value(list[0]).toString(), "Lorem");
    ASSERT_EQ(Value(list[1]).toString(), "ipsum");
    ASSERT_EQ(Value(list[2]).toString(), "dolor");
    ASSERT_EQ(Value(list[3]).toString(), "sit");
    ASSERT_EQ(Value(list[4]).toString(), "amet");
}

TEST(ListTest, ToString)
{
    List list("", "test list");
    std::string s;
    list.toString(s);
    ASSERT_EQ(s, "");
    ASSERT_EQ(list.toString(), "");

    list.append("");
    list.toString(s);
    ASSERT_EQ(s, "");
    ASSERT_EQ(list.toString(), "");

    list.append("");
    list.append("");
    ASSERT_EQ(list.toString(), "  ");

    list.clear();
    list.append("item1");
    list.append("i t e m 2");
    list.append("item 3");
    list.toString(s);
    ASSERT_EQ(s, "item1 i t e m 2 item 3");
    ASSERT_EQ(list.toString(), "item1 i t e m 2 item 3");

    list.clear();
    list.append(" ");
    list.append("a ");
    list.append(" b");
    list.append(" c ");
    list.toString(s);
    ASSERT_EQ(s, "  a   b  c ");
    ASSERT_EQ(list.toString(), "  a   b  c ");

    list.clear();
    list.append("áä");
    list.append("ľ š");
    list.toString(s);
    ASSERT_EQ(s, "áä ľ š");
    ASSERT_EQ(list.toString(), "áä ľ š");

    list.clear();
    list.append(-2);
    list.append(5);
    list.append(8);
    list.toString(s);
    ASSERT_EQ(s, "-2 5 8");
    ASSERT_EQ(list.toString(), "-2 5 8");

    list.clear();
    list.append(2);
    list.append(10);
    list.append(8);
    list.toString(s);
    ASSERT_EQ(s, "2 10 8");
    ASSERT_EQ(list.toString(), "2 10 8");

    list.clear();
    list.append(0);
    list.append(9);
    list.append(8);
    list.toString(s);
    ASSERT_EQ(s, "098");
    ASSERT_EQ(list.toString(), "098");
}

TEST(ListTest, Clone)
{
    List list("abc", "test list");
    list.append("Lorem");
    list.append("ipsum");
    list.append("dolor");
    list.append("sit");
    list.append("amet");

    std::shared_ptr<List> clone = list.clone();
    ASSERT_TRUE(clone);
    ASSERT_EQ(clone->id(), list.id());
    ASSERT_EQ(clone->name(), list.name());
    ASSERT_EQ(clone->size(), list.size());

    for (std::size_t i = 0; i < list.size(); i++)
        ASSERT_TRUE(value_equals(&list[i], &(*clone)[i]));
}
