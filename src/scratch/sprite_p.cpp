// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/costume.h>
#include <cmath>

#include "sprite_p.h"

using namespace libscratchcpp;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

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
        out->setLeft(0);
        out->setTop(0);
        out->setRight(0);
        out->setBottom(0);
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
