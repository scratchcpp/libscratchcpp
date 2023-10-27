// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "global.h"

namespace libscratchcpp
{

class IImageFormat;

/*! \brief The IImageFormatFactory class is an interface which is used to instantiate image formats. */
class LIBSCRATCHCPP_EXPORT IImageFormatFactory
{
    public:
        virtual ~IImageFormatFactory() { }

        /*! Creates an instance of the image format. */
        virtual std::shared_ptr<IImageFormat> createInstance() const = 0;
};

} // namespace libscratchcpp
