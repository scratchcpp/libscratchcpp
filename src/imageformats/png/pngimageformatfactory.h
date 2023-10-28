// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iimageformatfactory.h>

namespace libscratchcpp
{

class PngImageFormatFactory : public IImageFormatFactory
{
    public:
        PngImageFormatFactory();

        std::shared_ptr<IImageFormat> createInstance() const override;
};

} // namespace libscratchcpp
