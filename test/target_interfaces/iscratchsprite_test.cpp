#include "scratch/sprite.h"
#include "testspriteinterface.h"
#include "../common.h"

#define INIT_SPRITE() sprite.setInterface(&spriteInterface);

using namespace libscratchcpp;

Sprite sprite;
TestSpriteInterface spriteInterface;

TEST(IScratchSpriteTest, Visible)
{
    INIT_SPRITE();
    sprite.setVisible(false);
    ASSERT_EQ(spriteInterface.visible, false);
    sprite.setVisible(true);
    ASSERT_EQ(spriteInterface.visible, true);
}

TEST(IScratchSpriteTest, X)
{
    INIT_SPRITE();
    sprite.setX(300);
    ASSERT_EQ(spriteInterface.x, 300);
    sprite.setX(5.25);
    ASSERT_EQ(spriteInterface.x, 5.25);
}

TEST(IScratchSpriteTest, Y)
{
    INIT_SPRITE();
    sprite.setY(300);
    ASSERT_EQ(spriteInterface.y, 300);
    sprite.setY(5.25);
    ASSERT_EQ(spriteInterface.y, 5.25);
}

TEST(IScratchSpriteTest, Size)
{
    INIT_SPRITE();
    sprite.setSize(50);
    ASSERT_EQ(spriteInterface.size, 50);
    sprite.setSize(75.5);
    ASSERT_EQ(spriteInterface.size, 75.5);
}

TEST(IScratchSpriteTest, Direction)
{
    INIT_SPRITE();
    sprite.setDirection(85);
    ASSERT_EQ(spriteInterface.direction, 85);
    sprite.setDirection(179.715);
    ASSERT_EQ(spriteInterface.direction, 179.715);
}

TEST(IScratchSpriteTest, RotationStyle)
{
    INIT_SPRITE();
    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(spriteInterface.rotationStyle, Sprite::RotationStyle::DoNotRotate);
    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(spriteInterface.rotationStyle, Sprite::RotationStyle::LeftRight);
}

TEST(IScratchSpriteTest, CostumeData)
{
    INIT_SPRITE();
    // TODO: Add tests for costume data
}
