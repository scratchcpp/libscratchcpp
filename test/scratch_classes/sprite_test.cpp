#include "scratch/sprite.h"
#include "../common.h"

using namespace libscratchcpp;

TEST(SpriteTest, RotationStyle)
{
    Sprite sprite;
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // default
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");

    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");

    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
}
