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
