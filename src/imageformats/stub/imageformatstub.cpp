// SPDX-License-Identifier: Apache-2.0

#include "imageformatstub.h"

namespace libscratchcpp
{

ImageFormatStub::ImageFormatStub()
{
}

void ImageFormatStub::setData(unsigned int size, void *)
{
}

unsigned int ImageFormatStub::width() const
{
    return 0;
}

unsigned int ImageFormatStub::height() const
{
    return 0;
}

Rgb ImageFormatStub::colorAt(unsigned int, unsigned int, double) const
{
    return 0;
}

} // namespace libscratchcpp
