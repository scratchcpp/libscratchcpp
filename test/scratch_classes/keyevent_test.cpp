#include <scratchcpp/keyevent.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(KeyEventTest, Constructors)
{
    // Default constructor
    {
        KeyEvent event;
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_TRUE(event.name().empty());
    }

    // Type
    {
        KeyEvent event(KeyEvent::Type::Any);
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_TRUE(event.name().empty());
    }

    {
        KeyEvent event(KeyEvent::Type::Space);
        ASSERT_EQ(event.type(), KeyEvent::Type::Space);
        ASSERT_EQ(event.name(), "space");
    }

    {
        KeyEvent event(KeyEvent::Type::Left);
        ASSERT_EQ(event.type(), KeyEvent::Type::Left);
        ASSERT_EQ(event.name(), "left arrow");
    }

    {
        KeyEvent event(KeyEvent::Type::Up);
        ASSERT_EQ(event.type(), KeyEvent::Type::Up);
        ASSERT_EQ(event.name(), "up arrow");
    }

    {
        KeyEvent event(KeyEvent::Type::Right);
        ASSERT_EQ(event.type(), KeyEvent::Type::Right);
        ASSERT_EQ(event.name(), "right arrow");
    }

    {
        KeyEvent event(KeyEvent::Type::Down);
        ASSERT_EQ(event.type(), KeyEvent::Type::Down);
        ASSERT_EQ(event.name(), "down arrow");
    }

    {
        KeyEvent event(KeyEvent::Type::Enter);
        ASSERT_EQ(event.type(), KeyEvent::Type::Enter);
        ASSERT_EQ(event.name(), "enter");
    }

    {
        KeyEvent event(static_cast<KeyEvent::Type>(-1));
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_TRUE(event.name().empty());
    }

    // Name
    {
        KeyEvent event("");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_TRUE(event.name().empty());
    }

    {
        KeyEvent event("a");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "a");
    }

    {
        KeyEvent event("D");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "d");
    }

    {
        KeyEvent event("0");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "0");
    }

    {
        KeyEvent event("space");
        ASSERT_EQ(event.type(), KeyEvent::Type::Space);
        ASSERT_EQ(event.name(), "space");
    }

    {
        KeyEvent event("left arrow");
        ASSERT_EQ(event.type(), KeyEvent::Type::Left);
        ASSERT_EQ(event.name(), "left arrow");
    }

    {
        KeyEvent event("up arrow");
        ASSERT_EQ(event.type(), KeyEvent::Type::Up);
        ASSERT_EQ(event.name(), "up arrow");
    }

    {
        KeyEvent event("right arrow");
        ASSERT_EQ(event.type(), KeyEvent::Type::Right);
        ASSERT_EQ(event.name(), "right arrow");
    }

    {
        KeyEvent event("down arrow");
        ASSERT_EQ(event.type(), KeyEvent::Type::Down);
        ASSERT_EQ(event.name(), "down arrow");
    }

    {
        KeyEvent event("enter");
        ASSERT_EQ(event.type(), KeyEvent::Type::Enter);
        ASSERT_EQ(event.name(), "enter");
    }

    {
        KeyEvent event("Enter");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "e");
    }

    {
        KeyEvent event("test");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "t");
    }

    {
        KeyEvent event(" ");
        ASSERT_EQ(event.type(), KeyEvent::Type::Space);
        ASSERT_EQ(event.name(), "space");
    }

    {
        KeyEvent event(" a");
        ASSERT_EQ(event.type(), KeyEvent::Type::Space);
        ASSERT_EQ(event.name(), "space");
    }

    {
        KeyEvent event("33");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "3");
    }

    {
        KeyEvent event("47");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "4");
    }

    {
        KeyEvent event("48");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "0");
    }

    {
        KeyEvent event("64");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "@");
    }

    {
        KeyEvent event("77");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "m");
    }

    {
        KeyEvent event("90");
        ASSERT_EQ(event.type(), KeyEvent::Type::Any);
        ASSERT_EQ(event.name(), "z");
    }

    {
        KeyEvent event("32");
        ASSERT_EQ(event.type(), KeyEvent::Type::Space);
        ASSERT_EQ(event.name(), "space");
    }

    {
        KeyEvent event("37");
        ASSERT_EQ(event.type(), KeyEvent::Type::Left);
        ASSERT_EQ(event.name(), "left arrow");
    }

    {
        KeyEvent event("38");
        ASSERT_EQ(event.type(), KeyEvent::Type::Up);
        ASSERT_EQ(event.name(), "up arrow");
    }

    {
        KeyEvent event("39");
        ASSERT_EQ(event.type(), KeyEvent::Type::Right);
        ASSERT_EQ(event.name(), "right arrow");
    }

    {
        KeyEvent event("40");
        ASSERT_EQ(event.type(), KeyEvent::Type::Down);
        ASSERT_EQ(event.name(), "down arrow");
    }
}

TEST(KeyEventTest, EqualityOperator)
{
    KeyEvent ev1(KeyEvent::Type::Any);
    KeyEvent ev2(KeyEvent::Type::Any);
    KeyEvent ev3(KeyEvent::Type::Space);
    KeyEvent ev4(KeyEvent::Type::Space);
    KeyEvent ev5(KeyEvent::Type::Left);
    KeyEvent ev6(KeyEvent::Type::Left);
    KeyEvent ev7(KeyEvent::Type::Right);
    KeyEvent ev8(KeyEvent::Type::Right);
    KeyEvent ev9("a");
    KeyEvent ev10("A");
    KeyEvent ev11("8");
    KeyEvent ev12("8");
    KeyEvent ev13("space");

    ASSERT_TRUE(ev1 == ev2);
    ASSERT_TRUE(ev3 == ev4);
    ASSERT_TRUE(ev5 == ev6);
    ASSERT_TRUE(ev7 == ev8);
    ASSERT_TRUE(ev9 == ev10);
    ASSERT_TRUE(ev11 == ev12);
    ASSERT_TRUE(ev3 == ev13);

    ASSERT_FALSE(ev1 == ev3);
    ASSERT_FALSE(ev4 == ev6);
    ASSERT_FALSE(ev7 == ev10);
    ASSERT_FALSE(ev11 == ev13);
}
