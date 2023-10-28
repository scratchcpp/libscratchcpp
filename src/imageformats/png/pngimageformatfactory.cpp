// SPDX-License-Identifier: Apache-2.0

#include "pngimageformatfactory.h"
#include "pngimageformat.h"

using namespace libscratchcpp;

PngImageFormatFactory::PngImageFormatFactory()
{
}

std::shared_ptr<IImageFormat> PngImageFormatFactory::createInstance() const
{
    return std::make_shared<PngImageFormat>();
}
