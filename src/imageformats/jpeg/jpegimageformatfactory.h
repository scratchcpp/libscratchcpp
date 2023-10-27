// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iimageformatfactory.h>

namespace libscratchcpp
{

class JpegImageFormatFactory : public IImageFormatFactory
{
    public:
        JpegImageFormatFactory();

        std::shared_ptr<IImageFormat> createInstance() const override;
};

} // namespace libscratchcpp
