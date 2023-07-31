#include "testspriteinterface.h"

TestSpriteInterface::TestSpriteInterface()
{
}

void TestSpriteInterface::setVisible(bool visible)
{
    this->visible = visible;
}

void TestSpriteInterface::setX(double x)
{
    this->x = x;
}

void TestSpriteInterface::setY(double y)
{
    this->y = y;
}

void TestSpriteInterface::setSize(double size)
{
    this->size = size;
}

void TestSpriteInterface::setDirection(double direction)
{
    this->direction = direction;
}

void TestSpriteInterface::setRotationStyle(libscratchcpp::Sprite::RotationStyle rotationStyle)
{
    this->rotationStyle = rotationStyle;
}

void TestSpriteInterface::setCostume(const char *data)
{
    costumeData = data;
}
