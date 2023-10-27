// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

/*! A typedef for unsigned int. Holds the RGBA values. */
using Rgb = unsigned int;

/*! Returns the red component of the quadruplet rgb. */
inline constexpr int red(Rgb rgb)
{
    return ((rgb >> 16) & 0xff);
}

/*! Returns the green component of the quadruplet rgb. */
inline constexpr int green(Rgb rgb)
{
    return ((rgb >> 8) & 0xff);
}

/*! Returns the blue component of the quadruplet rgb. */
inline constexpr int blue(Rgb rgb)
{
    return (rgb & 0xff);
}

/*! Returns the alpha component of the quadruplet rgb. */
inline constexpr int alpha(Rgb rgb)
{
    return rgb >> 24;
}

/*! Creates an RGB triplet from the given color components. */
inline constexpr Rgb rgb(int r, int g, int b)
{
    return (0xffu << 24) | ((r & 0xffu) << 16) | ((g & 0xffu) << 8) | (b & 0xffu);
}

/*! Creates an RGBA quadruplet from the given color components. */
inline constexpr Rgb rgba(int r, int g, int b, int a)
{
    return ((a & 0xffu) << 24) | ((r & 0xffu) << 16) | ((g & 0xffu) << 8) | (b & 0xffu);
}

/*! \brief The IImageFormat class is an interface for image format implementation. */
class LIBSCRATCHCPP_EXPORT IImageFormat
{
    public:
        virtual ~IImageFormat() { }

        /*! Sets the image data (buffer). */
        virtual void setData(unsigned int size, void *data) = 0;

        /*! Returns the width of the image. */
        virtual unsigned int width() const = 0;

        /*! Returns the height of the image. */
        virtual unsigned int height() const = 0;

        /*! Returns the color at the given pixel. */
        virtual Rgb colorAt(unsigned int x, unsigned int y, double scale = 1) const = 0;
};

} // namespace libscratchcpp
