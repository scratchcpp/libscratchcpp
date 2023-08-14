#include <scratchcpp/broadcast.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(BroadcastTest, Constructors)
{
    Broadcast broadcast("a", "message1");
    ASSERT_EQ(broadcast.id(), "a");
    ASSERT_EQ(broadcast.name(), "message1");
}

TEST(BroadcastTest, Name)
{
    Broadcast broadcast("a", "message1");

    broadcast.setName("hello world");
    ASSERT_EQ(broadcast.name(), "hello world");
}
