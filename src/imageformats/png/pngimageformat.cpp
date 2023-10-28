// SPDX-License-Identifier: Apache-2.0

#include "pngimageformat.h"

using namespace libscratchcpp;

PngImageFormat::PngImageFormat()
{
}

PngImageFormat::~PngImageFormat()
{
    if (m_img)
        gdImageDestroy(m_img);
}

void PngImageFormat::setData(unsigned int size, void *data)
{
    if (m_img)
        gdImageDestroy(m_img);

    m_img = gdImageCreateFromPngPtr(size, data);
}

unsigned int PngImageFormat::width() const
{
    return m_img ? gdImageSX(m_img) : 0;
}

unsigned int PngImageFormat::height() const
{
    return m_img ? gdImageSY(m_img) : 0;
}

Rgb PngImageFormat::colorAt(unsigned int x, unsigned int y, double scale) const
{
    if (!m_img)
        return 0;

    int color = gdImageGetPixel(m_img, x / scale, y / scale);
    int alpha = 127 - gdImageAlpha(m_img, color); // gdImageAlpha() returns values from 0 to 127

    if (alpha == 127) // 127 should be the max (255)
        alpha = 255;
    else
        alpha *= 2;

    return rgba(gdImageRed(m_img, color), gdImageGreen(m_img, color), gdImageBlue(m_img, color), alpha);
}
