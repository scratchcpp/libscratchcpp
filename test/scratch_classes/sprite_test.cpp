#include <scratchcpp/sprite.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(SpriteTest, IsStage)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.isStage());
}

TEST(SpriteTest, Visible)
{
    Sprite sprite;
    ASSERT_TRUE(sprite.visible());
    sprite.setVisible(false);
    ASSERT_FALSE(sprite.visible());
}

TEST(SpriteTest, X)
{
    Sprite sprite;
    ASSERT_EQ(sprite.x(), 0);

    sprite.setX(-53.25);
    ASSERT_EQ(sprite.x(), -53.25);

    sprite.setX(239.999);
    ASSERT_EQ(sprite.x(), 239.999);
}

TEST(SpriteTest, Y)
{
    Sprite sprite;
    ASSERT_EQ(sprite.y(), 0);

    sprite.setY(-53.25);
    ASSERT_EQ(sprite.y(), -53.25);

    sprite.setY(179.999);
    ASSERT_EQ(sprite.y(), 179.999);
}

TEST(SpriteTest, Size)
{
    Sprite sprite;
    ASSERT_EQ(sprite.size(), 100);
    sprite.setSize(63.724);
    ASSERT_EQ(sprite.size(), 63.724);
}

TEST(SpriteTest, Direction)
{
    Sprite sprite;
    ASSERT_EQ(sprite.direction(), 90);

    sprite.setDirection(-42.75);
    ASSERT_EQ(sprite.direction(), -42.75);

    sprite.setDirection(145.2);
    ASSERT_EQ(sprite.direction(), 145.2);
}

TEST(SpriteTest, Draggable)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.draggable());
    sprite.setDraggable(true);
    ASSERT_TRUE(sprite.draggable());
}

TEST(SpriteTest, RotationStyle)
{
    Sprite sprite;
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // default
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");

    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");

    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle("invalid");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // shouldn't change
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle("don't rotate");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");

    sprite.setRotationStyle("left-right");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");

    sprite.setRotationStyle("all around");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle(std::string("invalid"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // shouldn't change
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle(std::string("don't rotate"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");

    sprite.setRotationStyle(std::string("left-right"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");

    sprite.setRotationStyle(std::string("all around"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
}
