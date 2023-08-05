#include "testspriteinterface.h"

TestSpriteInterface::TestSpriteInterface()
{
}

void TestSpriteInterface::onSpriteChanged(Sprite *sprite)
{
    this->sprite = sprite;
}

void TestSpriteInterface::onVisibleChanged(bool visible)
{
    this->visible = visible;
}

void TestSpriteInterface::onXChanged(double x)
{
    this->x = x;
}

void TestSpriteInterface::onYChanged(double y)
{
    this->y = y;
}

void TestSpriteInterface::onSizeChanged(double size)
{
    this->size = size;
}

void TestSpriteInterface::onDirectionChanged(double direction)
{
    this->direction = direction;
}

void TestSpriteInterface::onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle)
{
    this->rotationStyle = rotationStyle;
}

void TestSpriteInterface::onCostumeChanged(const char *data)
{
    costumeData = data;
}
