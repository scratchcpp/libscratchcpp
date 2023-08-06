#include <scratchcpp/blockprototype.h>
#include "../common.h"

using namespace libscratchcpp;

TEST(BlockPrototypeTest, Constructors)
{
    BlockPrototype p1;
    ASSERT_EQ(p1.procCode(), "");
    ASSERT_TRUE(p1.argumentTypes().empty());
    ASSERT_TRUE(p1.argumentDefaults().empty());
    ASSERT_FALSE(p1.warp());

    BlockPrototype p2("test block");
    ASSERT_EQ(p2.procCode(), "test block");
    ASSERT_TRUE(p2.argumentTypes().empty());
    ASSERT_TRUE(p2.argumentDefaults().empty());
    ASSERT_FALSE(p2.warp());

    BlockPrototype p3("test %s block %s %b");
    ASSERT_EQ(p3.procCode(), "test %s block %s %b");
    ASSERT_EQ(
        p3.argumentTypes(),
        std::vector<BlockPrototype::ArgType>({
            BlockPrototype::ArgType::StringNum,
            BlockPrototype::ArgType::StringNum,
            BlockPrototype::ArgType::Bool,
        }));
    ASSERT_EQ(p3.argumentDefaults(), std::vector<Value>({ "", "", false }));
    ASSERT_FALSE(p3.warp());
}

TEST(BlockPrototypeTest, ProcCode)
{
    BlockPrototype prototype;

    prototype.setProcCode("test block");
    ASSERT_EQ(prototype.procCode(), "test block");
    ASSERT_TRUE(prototype.argumentTypes().empty());
    ASSERT_TRUE(prototype.argumentDefaults().empty());

    prototype.setProcCode("test %s block %s %b");
    ASSERT_EQ(prototype.procCode(), "test %s block %s %b");
    ASSERT_EQ(
        prototype.argumentTypes(),
        std::vector<BlockPrototype::ArgType>({
            BlockPrototype::ArgType::StringNum,
            BlockPrototype::ArgType::StringNum,
            BlockPrototype::ArgType::Bool,
        }));
    ASSERT_EQ(prototype.argumentDefaults(), std::vector<Value>({ "", "", false }));

    prototype.setProcCode("%b %s test %b block %b");
    ASSERT_EQ(prototype.procCode(), "%b %s test %b block %b");
    ASSERT_EQ(
        prototype.argumentTypes(),
        std::vector<BlockPrototype::ArgType>({ BlockPrototype::ArgType::Bool, BlockPrototype::ArgType::StringNum, BlockPrototype::ArgType::Bool, BlockPrototype::ArgType::Bool }));
    ASSERT_EQ(prototype.argumentDefaults(), std::vector<Value>({ false, "", false, false }));
}

TEST(BlockPrototypeTest, ArgumentIds)
{
    BlockPrototype prototype;

    prototype.setArgumentIds({ "a", "b", "c" });
    ASSERT_EQ(prototype.argumentIds(), std::vector<std::string>({ "a", "b", "c" }));
}

TEST(BlockPrototypeTest, ArgumentNames)
{
    BlockPrototype prototype;

    prototype.setArgumentNames({ "arg1", "arg2", "arg3" });
    ASSERT_EQ(prototype.argumentNames(), std::vector<std::string>({ "arg1", "arg2", "arg3" }));
}

TEST(BlockPrototypeTest, Warp)
{
    BlockPrototype prototype;

    prototype.setWarp(true);
    ASSERT_TRUE(prototype.warp());
}
