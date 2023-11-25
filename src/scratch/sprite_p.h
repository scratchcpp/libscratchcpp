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

        void getBoundingRect(Rect *out) const;
        void getFencedPosition(double inX, double inY, double *outX, double *outY) const;

        Sprite *sprite = nullptr;
        ISpriteHandler *iface = nullptr;
        Sprite *cloneRoot = nullptr;
        Sprite *cloneParent = nullptr;
        std::vector<std::shared_ptr<Sprite>> childClones;
        bool visible = true;
        double x = 0;
        double y = 0;
        double size = 100;
        double direction = 90;
        bool draggable = false;
        Sprite::RotationStyle rotationStyle = Sprite::RotationStyle::AllAround;
        std::unordered_map<IGraphicsEffect *, double> graphicsEffects;
};

} // namespace libscratchcpp
