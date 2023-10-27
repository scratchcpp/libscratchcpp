// SPDX-License-Identifier: Apache-2.0

#include "jpegimageformatfactory.h"
#include "jpegimageformat.h"

using namespace libscratchcpp;

JpegImageFormatFactory::JpegImageFormatFactory()
{
}

std::shared_ptr<IImageFormat> JpegImageFormatFactory::createInstance() const
{
    return std::make_shared<JpegImageFormat>();
}
