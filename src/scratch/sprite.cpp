// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <scratchcpp/ispritehandler.h>

#include "sprite.h"

using namespace libscratchcpp;

/*! Sets the sprite interface. */
void Sprite::setInterface(ISpriteHandler *newInterface)
{
    assert(newInterface);
    m_iface = newInterface;
    m_iface->onSpriteChanged(this);
}

/*! Returns true if the sprite is visible. */
bool Sprite::visible() const
{
    return m_visible;
}

/*! Sets the visibility of the sprite. */
void Sprite::setVisible(bool newVisible)
{
    m_visible = newVisible;
    if (m_iface)
        m_iface->onVisibleChanged(m_visible);
}

/*! Returns the X position of the sprite. */
double Sprite::x() const
{
    return m_x;
}

/*! Sets the X position of the sprite. */
void Sprite::setX(double newX)
{
    m_x = newX;
    if (m_iface)
        m_iface->onXChanged(m_x);
}

/*! Returns the Y position of the sprite. */
double Sprite::y() const
{
    return m_y;
}

/*! Sets the Y position of the sprite. */
void Sprite::setY(double newY)
{
    m_y = newY;
    if (m_iface)
        m_iface->onYChanged(m_y);
}

/*! Returns the size. */
double Sprite::size() const
{
    return m_size;
}

/*! Sets the size. */
void Sprite::setSize(double newSize)
{
    m_size = newSize;
    if (m_iface)
        m_iface->onSizeChanged(m_size);
}

/*! Returns the direction. */
double Sprite::direction() const
{
    return m_direction;
}

/*! Sets the direction. */
void Sprite::setDirection(double newDirection)
{
    m_direction = newDirection;
    if (m_iface)
        m_iface->onDirectionChanged(m_direction);
}

/*! Returns true if the sprite is draggable. */
bool Sprite::draggable() const
{
    return m_draggable;
}

/*! Toggles whether the sprite is draggable. */
void Sprite::setDraggable(bool newDraggable)
{
    m_draggable = newDraggable;
}

/*! Returns the rotation style. */
Sprite::RotationStyle Sprite::rotationStyle() const
{
    return m_rotationStyle;
}

/*! Returns the rotation style as a string. */
std::string Sprite::rotationStyleStr() const
{
    switch (m_rotationStyle) {
        case RotationStyle::AllAround:
            return "all around";
        case RotationStyle::LeftRight:
            return "left-right";
        case RotationStyle::DoNotRotate:
            return "don't rotate";
    }
    return "all around";
}

/*! Sets the rotation style. */
void Sprite::setRotationStyle(RotationStyle newRotationStyle)
{
    m_rotationStyle = newRotationStyle;
    if (m_iface)
        m_iface->onRotationStyleChanged(m_rotationStyle);
}

/*! \copydoc setRotationStyle() */
void Sprite::setRotationStyle(const std::string &newRotationStyle)
{
    if (newRotationStyle == "all around")
        setRotationStyle(RotationStyle::AllAround);
    else if (newRotationStyle == "left-right")
        setRotationStyle(RotationStyle::LeftRight);
    else if (newRotationStyle == "don't rotate")
        setRotationStyle(RotationStyle::DoNotRotate);
}

/*! \copydoc setRotationStyle() */
void Sprite::setRotationStyle(const char *newRotationStyle)
{
    setRotationStyle(std::string(newRotationStyle));
}

void Sprite::setCostumeData(const char *data)
{
    if (m_iface)
        m_iface->onCostumeChanged(data);
}
