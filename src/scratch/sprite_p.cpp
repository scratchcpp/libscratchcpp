// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/iengine.h>
#include <cmath>

#include "sprite_p.h"

using namespace libscratchcpp;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20
static const double FENCE_WIDTH = 15;

SpritePrivate::SpritePrivate(Sprite *sprite) :
    sprite(sprite)
{
}

void SpritePrivate::removeClone(Sprite *clone)
{
    int index = 0;
    for (const auto &child : clones) {
        if (child.get() == clone) {
            clones.erase(clones.begin() + index);
            return;
        }

        index++;
    }
}

void SpritePrivate::getFencedPosition(double x, double y, double *outX, double *outY) const
{
    assert(outX);
    assert(outY);

    if (!sprite->engine()) {
        *outX = x;
        *outY = y;
        return;
    }

    // https://github.com/scratchfoundation/scratch-render/blob/0b51e5a66ae1c8102fe881107145d7ef3d71a1ab/src/RenderWebGL.js#L1526
    double dx = x - this->x;
    double dy = y - this->y;
    Rect rect;

    if (iface)
        rect = iface->boundingRect();

    double inset = std::floor(std::min(rect.width(), rect.height()) / 2);

    double xRight = static_cast<double>(sprite->engine()->stageWidth()) / 2;
    double sx = xRight - std::min(FENCE_WIDTH, inset);

    if (rect.right() + dx < -sx) {
        x = std::ceil(this->x - (sx + rect.right()));
    } else if (rect.left() + dx > sx) {
        x = std::floor(this->x + (sx - rect.left()));
    }

    double yTop = static_cast<double>(sprite->engine()->stageHeight()) / 2;
    double sy = yTop - std::min(FENCE_WIDTH, inset);

    if (rect.top() + dy < -sy) {
        y = std::ceil(this->y - (sy + rect.top()));
    } else if (rect.bottom() + dy > sy) {
        y = std::floor(this->y + (sy - rect.bottom()));
    }

    *outX = x;
    *outY = y;
}
