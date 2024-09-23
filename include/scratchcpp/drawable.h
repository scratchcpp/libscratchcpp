// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class DrawablePrivate;

/*! \brief The Drawable class is the base class of rendered elements (stage, sprites, text bubbles). */
class LIBSCRATCHCPP_EXPORT Drawable
{
    public:
        Drawable();
        Drawable(const Drawable &) = delete;

        int layerOrder() const;
        virtual void setLayerOrder(int newLayerOrder);

    private:
        spimpl::unique_impl_ptr<DrawablePrivate> impl;
};

} // namespace libscratchcpp
