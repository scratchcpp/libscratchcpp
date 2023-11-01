// SPDX-License-Identifier: Apache-2.0

#include "rect_p.h"

namespace libscratchcpp
{

RectPrivate::RectPrivate(double left, double top, double right, double bottom) :
    left(left),
    top(top),
    right(right),
    bottom(bottom)
{
}

RectPrivate::RectPrivate()
{
}

} // namespace libscratchcpp
