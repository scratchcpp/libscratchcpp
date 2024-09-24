// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/drawable.h>

#include "drawable_p.h"

using namespace libscratchcpp;

/*! Constructs Drawable. */
Drawable::Drawable() :
    impl(spimpl::make_unique_impl<DrawablePrivate>())
{
}

/*! Returns the layer number. */
int Drawable::layerOrder() const
{
    return impl->layerOrder;
}

/*! Sets the layer number. */
void Drawable::setLayerOrder(int newLayerOrder)
{
    impl->layerOrder = newLayerOrder;
}

/*! Returns the engine. */
IEngine *Drawable::engine() const
{
    return impl->engine;
}

/*! Sets the engine. */
void Drawable::setEngine(IEngine *engine)
{
    impl->engine = engine;
}
