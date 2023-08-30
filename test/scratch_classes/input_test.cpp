#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>

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

    input.setValueBlock(block);
    input.setValueBlock(nullptr);
    ASSERT_EQ(input.valueBlock(), nullptr);
    ASSERT_EQ(input.valueBlockId(), "");
}

TEST(InputTest, SelectedMenuItem)
{
    // Test with Shadow type
    Input input1("", Input::Type::Shadow);
    ASSERT_FALSE(input1.pointsToDropdownMenu());
    ASSERT_TRUE(input1.selectedMenuItem().empty());

    auto block1 = std::make_shared<Block>("abc", "");
    input1.setValueBlock(block1);
    ASSERT_FALSE(input1.pointsToDropdownMenu());
    ASSERT_TRUE(input1.selectedMenuItem().empty());

    auto field1 = std::make_shared<Field>("OPTION1", "");
    block1->addField(field1);
    ASSERT_TRUE(input1.pointsToDropdownMenu());
    ASSERT_TRUE(input1.selectedMenuItem().empty());

    auto field2 = std::make_shared<Field>("OPTION2", "something");
    block1->addField(field2);
    ASSERT_FALSE(input1.pointsToDropdownMenu());
    ASSERT_TRUE(input1.selectedMenuItem().empty());

    auto block2 = std::make_shared<Block>("def", "");
    input1.setValueBlock(block2);

    field1 = std::make_shared<Field>("OPTION1", "something");
    block2->addField(field1);
    ASSERT_TRUE(input1.pointsToDropdownMenu());
    ASSERT_EQ(input1.selectedMenuItem(), "something");

    auto variable = std::make_shared<Variable>("", "");
    field1->setValuePtr(variable);
    ASSERT_FALSE(input1.pointsToDropdownMenu());
    ASSERT_TRUE(input1.selectedMenuItem().empty());

    field1->setValuePtr(nullptr);
    ASSERT_TRUE(input1.pointsToDropdownMenu());
    ASSERT_EQ(input1.selectedMenuItem(), "something");

    field2 = std::make_shared<Field>("OPTION2", "hello");
    block2->addField(field2);
    ASSERT_FALSE(input1.pointsToDropdownMenu());
    ASSERT_TRUE(input1.selectedMenuItem().empty());

    // Test with NoShadow type
    Input input2("", Input::Type::NoShadow);
    ASSERT_FALSE(input2.pointsToDropdownMenu());
    ASSERT_TRUE(input2.selectedMenuItem().empty());

    auto block3 = std::make_shared<Block>("ghi", "");
    input2.setValueBlock(block3);
    ASSERT_FALSE(input2.pointsToDropdownMenu());
    ASSERT_TRUE(input2.selectedMenuItem().empty());

    block3->addField(field1);
    ASSERT_FALSE(input2.pointsToDropdownMenu());
    ASSERT_TRUE(input2.selectedMenuItem().empty());

    // Test with ObscuredShadow type
    Input input3("", Input::Type::ObscuredShadow);
    ASSERT_FALSE(input3.pointsToDropdownMenu());
    ASSERT_TRUE(input3.selectedMenuItem().empty());

    input3.setValueBlock(block3);
    ASSERT_FALSE(input3.pointsToDropdownMenu());
    ASSERT_TRUE(input3.selectedMenuItem().empty());

    block3->addField(field1);
    ASSERT_FALSE(input3.pointsToDropdownMenu());
    ASSERT_TRUE(input3.selectedMenuItem().empty());
}
