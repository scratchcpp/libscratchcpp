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
    for (const auto &child : childClones) {
        if (child.get() == clone) {
            childClones.erase(childClones.begin() + index);
            return;
        }

        index++;
    }
}

void SpritePrivate::setCostumeData(const char *data)
{
    if (iface)
        iface->onCostumeChanged(data);
}

void SpritePrivate::getBoundingRect(Rect *out) const
{
    assert(out);
    assert(sprite);
    // TODO: Make currentCostume() return the costume
    auto costume = sprite->costumeAt(sprite->currentCostume() - 1);

    if (!costume) {
        out->setLeft(x);
        out->setTop(y);
        out->setRight(x);
        out->setBottom(y);
        return;
    }

    double cosTheta = std::cos((90 - direction) * pi / 180);
    double sinTheta = std::sin((90 - direction) * pi / 180);
    double maxX = 0, maxY = 0, minX = 0, minY = 0;
    bool firstPixel = true;
    unsigned int width = costume->width();
    unsigned int height = costume->height();
    double rotationCenterX = width / 2.0 + costume->rotationCenterX();
    double rotationCenterY = height / 2.0 + costume->rotationCenterY();
    Rgb **bitmap = costume->bitmap();

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            if (bitmap[y][x] != rgba(0, 0, 0, 0)) {
                double rotatedX = ((x - rotationCenterX) * cosTheta - (y - rotationCenterY) * sinTheta);
                double rotatedY = ((x - rotationCenterX) * sinTheta + (y - rotationCenterY) * cosTheta);

                if (firstPixel) {
                    firstPixel = false;
                    minX = maxX = rotatedX;
                    minY = maxY = rotatedY;
                } else {
                    if (rotatedX < minX)
                        minX = rotatedX;
                    else if (rotatedX > maxX)
                        maxX = rotatedX;

                    if (rotatedY < minY)
                        minY = rotatedY;
                    else if (rotatedY > maxY)
                        maxY = rotatedY;
                }
            }
        }
    }

    out->setLeft(x + minX);
    out->setTop(y + maxY);
    out->setRight(x + maxX);
    out->setBottom(y + minY);
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
    getBoundingRect(&rect);
    double inset = std::floor(std::min(rect.width(), rect.height()) / 2);

    double xRight = sprite->engine()->stageWidth() / 2;
    double sx = xRight - std::min(FENCE_WIDTH, inset);

    if (rect.right() + dx < -sx) {
        x = std::ceil(this->x - (sx + rect.right()));
    } else if (rect.left() + dx > sx) {
        x = std::floor(this->x + (sx - rect.left()));
    }

    double yTop = sprite->engine()->stageHeight() / 2;
    double sy = yTop - std::min(FENCE_WIDTH, inset);

    if (rect.top() + dy < -sy) {
        y = std::ceil(this->y - (sy + rect.top()));
    } else if (rect.bottom() + dy > sy) {
        y = std::floor(this->y + (sy - rect.bottom()));
    }

    *outX = x;
    *outY = y;
}
