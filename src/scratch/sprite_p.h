// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/sprite.h>
#include <unordered_map>

namespace libscratchcpp
{

class Rect;

struct SpritePrivate
{
        SpritePrivate(Sprite *sprite);
        SpritePrivate(const SpritePrivate &) = delete;

        void removeClone(Sprite *clone);

        void getFencedPosition(double inX, double inY, double *outX, double *outY) const;

        Sprite *sprite = nullptr;
        ISpriteHandler *iface = nullptr;
        Sprite *cloneSprite = nullptr;
        std::vector<std::shared_ptr<Sprite>> clones;
        bool cloneDeleted = false;
        bool visible = true;
        double x = 0;
        double y = 0;
        double size = 100;
        double direction = 90;
        bool draggable = false;
        bool dragging = false;
        Sprite::RotationStyle rotationStyle = Sprite::RotationStyle::AllAround;
};

} // namespace libscratchcpp
