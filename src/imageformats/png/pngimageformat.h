// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iimageformat.h>
#include <gd.h>

namespace libscratchcpp
{

class PngImageFormat : public IImageFormat
{
    public:
        PngImageFormat();
        ~PngImageFormat();

        void setData(unsigned int size, void *data) override;

        unsigned int width() const override;
        unsigned int height() const override;

        Rgb colorAt(unsigned int x, unsigned int y, double scale) const override;

    private:
        gdImagePtr m_img = nullptr;
};

} // namespace libscratchcpp
