#include <scratchcpp/costume.h>
#include <scratchcpp/broadcast.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(CostumeTest, Constructors)
{
    Costume costume("costume1", "a", "svg");
    ASSERT_EQ(costume.id(), "a");
    ASSERT_EQ(costume.name(), "costume1");
    ASSERT_EQ(costume.dataFormat(), "svg");
    ASSERT_EQ(costume.fileName(), "a.svg");
    ASSERT_EQ(costume.bitmapResolution(), 1);
    ASSERT_EQ(costume.rotationCenterX(), 0);
    ASSERT_EQ(costume.rotationCenterY(), 0);
}

TEST(CostumeTest, BitmapResolution)
{
    Costume costume("costume1", "a", "svg");

    costume.setBitmapResolution(5.52);
    ASSERT_EQ(costume.bitmapResolution(), 5.52);
}

TEST(CostumeTest, RotationCenterX)
{
    Costume costume("costume1", "a", "svg");

    costume.setRotationCenterX(240);
    ASSERT_EQ(costume.rotationCenterX(), 240);
}

TEST(CostumeTest, RotationCenterY)
{
    Costume costume("costume1", "a", "svg");

    costume.setRotationCenterY(180);
    ASSERT_EQ(costume.rotationCenterY(), 180);
}

TEST(CostumeTest, Broadcast)
{
    Costume costume("costume1", "a", "svg");

    Broadcast *broadcast = costume.broadcast();
    ASSERT_TRUE(broadcast);
    ASSERT_TRUE(broadcast->id().empty());
    ASSERT_TRUE(broadcast->name().empty());
}
