// SPDX-License-Identifier: Apache-2.0

#include "costume.h"

using namespace libscratchcpp;

/*! Constructs Costume. */
Costume::Costume(std::string name, std::string id, std::string format) :
    Asset(name, id, format)
{
}

/*! Returns the reciprocal of the costume scaling factor for bitmap costumes. */
double Costume::bitmapResolution() const
{
    return m_bitmapResolution;
}

/*! Sets the reciprocal of the costume scaling factor for bitmap costumes. */
void Costume::setBitmapResolution(double newBitmapResolution)
{
    m_bitmapResolution = newBitmapResolution;
}

/*! Returns the x-coordinate of the rotation center. */
int Costume::rotationCenterX() const
{
    return m_rotationCenterX;
}

/*! Sets the x-coordinate of the rotation center. */
void Costume::setRotationCenterX(int newRotationCenterX)
{
    m_rotationCenterX = newRotationCenterX;
}

/*! Returns the y-coordinate of the rotation center. */
int Costume::rotationCenterY() const
{
    return m_rotationCenterY;
}

/*! Returns the y-coordinate of the rotation center. */
void Costume::setRotationCenterY(int newRotationCenterY)
{
    m_rotationCenterY = newRotationCenterY;
}
