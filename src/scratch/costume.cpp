// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/costume.h>
#include <scratchcpp/scratchconfiguration.h>

#include "costume_p.h"

using namespace libscratchcpp;

/*! Constructs Costume. */
Costume::Costume(const std::string &name, const std::string &id, const std::string &format) :
    Asset(name, id, format),
    impl(spimpl::make_unique_impl<CostumePrivate>())
{
    // NOTE: image is always initialized, so there's no need for null checks
    impl->image = ScratchConfiguration::createImageFormat(format);
    impl->updateImage();
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

/*! Returns the costume width. */
unsigned int Costume::width() const
{
    return impl->image->width() * impl->scale;
}

/*! Returns the costume height. */
unsigned int Costume::height() const
{
    return impl->image->height() * impl->scale;
}

/*! Returns the image scale. */
double Costume::scale() const
{
    return impl->scale;
}

/*! Sets the image scale (this is automatically set by the sprite). */
void Costume::setScale(double scale)
{
    if (impl->scale == scale)
        return;

    impl->scale = scale;
    impl->updateImage();
}

/*! Returns true if the costume image is mirrored horizontally. */
bool Costume::mirrorHorizontally() const
{
    return impl->mirrorHorizontally;
}

/*! Sets whether the costume image is mirrored horizontally (this is automatically set by the sprite). */
void Costume::setMirrorHorizontally(bool mirror)
{
    if (impl->mirrorHorizontally == mirror)
        return;

    impl->mirrorHorizontally = mirror;
    impl->updateImage();
}

/*! Returns the bitmap of the costume (an array of pixel rows). */
Rgb **Costume::bitmap() const
{
    return impl->bitmap;
}

/*!
 * Returns the Broadcast linked with this costume.
 * \note This is used by the "switch backdrop to and wait" block.
 */
Broadcast *Costume::broadcast()
{
    return &impl->broadcast;
}

void Costume::processData(const char *data)
{
    impl->image->setData(data);
    impl->freeImage();
    impl->updateImage();
}
