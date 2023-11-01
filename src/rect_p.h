// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

struct RectPrivate
{
        RectPrivate(double left, double top, double right, double bottom);
        RectPrivate();

        double left = 0;
        double top = 0;
        double right = 0;
        double bottom = 0;
};

} // namespace libscratchcpp
