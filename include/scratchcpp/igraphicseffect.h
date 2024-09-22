// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "global.h"

namespace libscratchcpp
{

/*! \brief The IGraphicsEffects class is an interface for custom graphics effects. */
class LIBSCRATCHCPP_EXPORT IGraphicsEffect
{
    public:
        virtual ~IGraphicsEffect() { }

        /*! Returns the name of the graphics effect. */
        virtual std::string name() const = 0;

        /*! Returns the clamped value of the graphic effect. */
        virtual double clamp(double value) const = 0;
};

} // namespace libscratchcpp
