// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iimageformat.h>

namespace libscratchcpp
{

class ImageFormatStub : public IImageFormat
{
    public:
        ImageFormatStub();

        void setData(const char *) override;

        unsigned int width() const override;
        unsigned int height() const override;

        Rgb colorAt(unsigned int, unsigned int, double) const override;
};

} // namespace libscratchcpp
