#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/block.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(InputTest, Constructors)
{
    Input input1("VALUE", Input::Type::Shadow);
    ASSERT_EQ(input1.name(), "VALUE");
    ASSERT_EQ(input1.type(), Input::Type::Shadow);

    Input input2("custom_block", Input::Type::NoShadow);
    ASSERT_EQ(input2.name(), "custom_block");
    ASSERT_EQ(input2.type(), Input::Type::NoShadow);
}

TEST(InputTest, InputId)
{
    Input input("", Input::Type::Shadow);
    ASSERT_EQ(input.inputId(), -1);

    input.setInputId(3);
    ASSERT_EQ(input.inputId(), 3);
}

TEST(InputTest, PrimaryValue)
{
    Input input("", Input::Type::Shadow);
    ASSERT_TRUE(input.primaryValue());

    input.setPrimaryValue("test");
    ASSERT_EQ(input.primaryValue()->value().toString(), "test");
}

TEST(InputTest, SecondaryValue)
{
    Input input("", Input::Type::Shadow);
    ASSERT_TRUE(input.secondaryValue());

    input.setSecondaryValue("test");
    ASSERT_EQ(input.secondaryValue()->value().toString(), "test");
}

TEST(InputTest, ValueBlock)
{
    Input input("", Input::Type::Shadow);
    ASSERT_EQ(input.valueBlock(), nullptr);
    ASSERT_EQ(input.valueBlockId(), "");

    auto block = std::make_shared<Block>("abc", "");
    input.setValueBlock(block);
    ASSERT_EQ(input.valueBlock(), block);
    ASSERT_EQ(input.valueBlockId(), "abc");

    input.setValueBlockId("hello");
    ASSERT_EQ(input.valueBlockId(), "hello");
    ASSERT_EQ(input.valueBlock(), nullptr);
}
