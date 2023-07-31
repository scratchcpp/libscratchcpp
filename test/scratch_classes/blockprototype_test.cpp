#include "scratch/blockprototype.h"
#include "../common.h"

using namespace libscratchcpp;

TEST(BlockPrototypeTest, ProcCode)
{
    BlockPrototype prototype("test block");
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
