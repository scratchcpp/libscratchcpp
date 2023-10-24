// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

/*! A typedef for unsigned int. Holds the RGBA values. */
using Rgb = unsigned int;

/*! \brief The IImageFormat class is an interface for image format implementation. */
class LIBSCRATCHCPP_EXPORT IImageFormat
{
    public:
        virtual ~IImageFormat() { }

        /*! Sets the image data (buffer). */
        virtual void setData(const char *data) = 0;

        /*! Returns the width of the image. */
        virtual unsigned int width() const = 0;

        /*! Returns the height of the image. */
        virtual unsigned int height() const = 0;

        /*! Returns the color at the given pixel. */
        virtual Rgb colorAt(unsigned int x, unsigned int y, double scale = 1) const = 0;
};

} // namespace libscratchcpp
