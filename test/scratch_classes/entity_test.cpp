#include <scratchcpp/entity.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(EntityTest, Constructors)
{
    Entity entity("abc");
    ASSERT_EQ(entity.id(), "abc");
}

TEST(EntityTest, Id)
{
    Entity entity("abc");

    entity.setId("def");
    ASSERT_EQ(entity.id(), "def");
}
