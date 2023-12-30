#include <scratchcpp/costume.h>
#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/broadcast.h>

#include "../common.h"

using namespace libscratchcpp;

class CostumeTest : public testing::Test
{
    public:
        void SetUp() override { }
};

TEST_F(CostumeTest, Constructors)
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

TEST_F(CostumeTest, BitmapResolution)
{
    Costume costume("costume1", "a", "svg");

    costume.setBitmapResolution(5.52);
    ASSERT_EQ(costume.bitmapResolution(), 5.52);
}

TEST_F(CostumeTest, RotationCenterX)
{
    Costume costume("costume1", "a", "svg");

    costume.setRotationCenterX(240);
    ASSERT_EQ(costume.rotationCenterX(), 240);
}

TEST_F(CostumeTest, RotationCenterY)
{
    Costume costume("costume1", "a", "svg");

    costume.setRotationCenterY(180);
    ASSERT_EQ(costume.rotationCenterY(), 180);
}

TEST_F(CostumeTest, Broadcast)
{
    Costume costume("costume1", "a", "svg");

    Broadcast *broadcast = costume.broadcast();
    ASSERT_TRUE(broadcast);
    ASSERT_TRUE(broadcast->id().empty());
    ASSERT_EQ(broadcast->name(), "costume1");
}
