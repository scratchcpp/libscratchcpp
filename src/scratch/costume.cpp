// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/costume.h>

#include "costume_p.h"

using namespace libscratchcpp;

/*! Constructs Costume. */
Costume::Costume(const std::string &name, const std::string &id, const std::string &format) :
    Asset(name, id, format),
    impl(spimpl::make_unique_impl<CostumePrivate>())
{
}

/*! Returns the reciprocal of the costume scaling factor for bitmap costumes. */
double Costume::bitmapResolution() const
{
    return impl->bitmapResolution;
}

/*! Sets the reciprocal of the costume scaling factor for bitmap costumes. */
void Costume::setBitmapResolution(double newBitmapResolution)
{
    impl->bitmapResolution = newBitmapResolution;
}

/*! Returns the x-coordinate of the rotation center. */
int Costume::rotationCenterX() const
{
    return impl->rotationCenterX;
}

/*! Sets the x-coordinate of the rotation center. */
void Costume::setRotationCenterX(int newRotationCenterX)
{
    impl->rotationCenterX = newRotationCenterX;
}

/*! Returns the y-coordinate of the rotation center. */
int Costume::rotationCenterY() const
{
    return impl->rotationCenterY;
}

/*! Returns the y-coordinate of the rotation center. */
void Costume::setRotationCenterY(int newRotationCenterY)
{
    impl->rotationCenterY = newRotationCenterY;
}

/*!
 * Returns the Broadcast linked with this costume.
 * \note This is used by the "switch backdrop to and wait" block.
 */
Broadcast *Costume::broadcast()
{
    return &impl->broadcast;
}
