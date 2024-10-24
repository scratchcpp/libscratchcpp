// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "signal.h"
#include "spimpl.h"

namespace libscratchcpp
{

class IEngine;

class DrawablePrivate;

/*! \brief The Drawable class is the base class of rendered elements (stage, sprites, text bubbles). */
class LIBSCRATCHCPP_EXPORT Drawable
{
    public:
        Drawable();
        Drawable(const Drawable &) = delete;

        /*! Returns true if this Drawable is a Target. */
        virtual bool isTarget() const { return false; }

        /*! Returns true if this Drawable is a TextBubble. */
        virtual bool isTextBubble() const { return false; }

        int layerOrder() const;
        virtual void setLayerOrder(int newLayerOrder);
        sigslot::signal<int> &layerOrderChanged() const;

        IEngine *engine() const;
        virtual void setEngine(IEngine *engine);

    private:
        spimpl::unique_impl_ptr<DrawablePrivate> impl;
};

} // namespace libscratchcpp
