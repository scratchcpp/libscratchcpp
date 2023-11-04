// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "iimageformat.h"

namespace libscratchcpp
{

/*! \brief The IGraphicsEffects class is an interface for implementing custom graphics effects. */
class LIBSCRATCHCPP_EXPORT IGraphicsEffect
{
    public:
        virtual ~IGraphicsEffect() { }

        /*! Returns the name of the graphics effect. */
        virtual std::string name() const = 0;

        /*! Applies the effect on the given bitmap. */
        virtual void apply(Rgb **bitmap, unsigned int width, unsigned int height, double value) const = 0;
};

} // namespace libscratchcpp
