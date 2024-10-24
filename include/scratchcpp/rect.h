// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class RectPrivate;

/*! The Rect class represents a rectangle. */
class LIBSCRATCHCPP_EXPORT Rect
{
    public:
        Rect(double left, double top, double right, double bottom);
        Rect();

        double left() const;
        void setLeft(double left);

        double top() const;
        void setTop(double top);

        double right() const;
        void setRight(double right);

        double bottom() const;
        void setBottom(double bottom);

        double width() const;
        double height() const;

        void clamp(double left, double top, double right, double bottom);
        void snapToInt();

        bool intersects(const Rect &rect) const;
        bool contains(double x, double y) const;

        static void intersection(const Rect &a, const Rect &b, Rect &dst);
        static void united(const Rect &a, const Rect &b, Rect &dst);

    private:
        spimpl::impl_ptr<RectPrivate> impl;
};

} // namespace libscratchcpp
