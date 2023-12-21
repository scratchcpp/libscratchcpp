// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "target.h"

namespace libscratchcpp
{

class ISpriteHandler;
class Rect;
class IGraphicsEffect;
class SpritePrivate;

/*! \brief The Sprite class represents a Scratch sprite. */
class LIBSCRATCHCPP_EXPORT Sprite : public Target
{
    public:
        enum class RotationStyle
        {
            AllAround,
            LeftRight,
            DoNotRotate
        };

        Sprite();
        Sprite(const Sprite &) = delete;

        void setInterface(ISpriteHandler *newInterface);

        std::shared_ptr<Sprite> clone();
        void deleteClone();

        bool isClone() const;

        Sprite *cloneSprite() const;
        const std::vector<std::shared_ptr<Sprite>> &clones() const;

        bool visible() const;
        void setVisible(bool newVisible);

        double x() const;
        void setX(double newX);

        double y() const;
        void setY(double newY);

        double size() const;
        void setSize(double newSize);

        void setCostumeIndex(int newCostumeIndex) override;

        double direction() const;
        void setDirection(double newDirection);

        bool draggable() const;
        void setDraggable(bool newDraggable);

        RotationStyle rotationStyle() const;
        std::string rotationStyleStr() const;
        void setRotationStyle(RotationStyle newRotationStyle);
        void setRotationStyle(const std::string &newRotationStyle);
        void setRotationStyle(const char *newRotationStyle);

        Rect boundingRect() const;
        void keepInFence(double newX, double newY, double *fencedX, double *fencedY) const;

        void setGraphicsEffectValue(IGraphicsEffect *effect, double value) override;

        void clearGraphicsEffects() override;

    private:
        Target *dataSource() const override;
        void setXY(double x, double y);

        spimpl::unique_impl_ptr<SpritePrivate> impl;
};

} // namespace libscratchcpp
