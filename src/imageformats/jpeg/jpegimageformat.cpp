// SPDX-License-Identifier: Apache-2.0

#include "jpegimageformat.h"

using namespace libscratchcpp;

JpegImageFormat::JpegImageFormat()
{
}

JpegImageFormat::~JpegImageFormat()
{
    if (m_img)
        gdImageDestroy(m_img);
}

void JpegImageFormat::setData(unsigned int size, void *data)
{
    if (m_img)
        gdImageDestroy(m_img);

    m_img = gdImageCreateFromJpegPtr(size, data);
}

unsigned int JpegImageFormat::width() const
{
    return m_img ? gdImageSX(m_img) : 0;
}

unsigned int JpegImageFormat::height() const
{
    return m_img ? gdImageSY(m_img) : 0;
}

Rgb JpegImageFormat::colorAt(unsigned int x, unsigned int y, double scale) const
{
    if (!m_img)
        return 0;

    int color = gdImageGetPixel(m_img, x / scale, y / scale);

    return rgb(gdImageRed(m_img, color), gdImageGreen(m_img, color), gdImageBlue(m_img, color));
}
