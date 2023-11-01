// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "target.h"

namespace libscratchcpp
{

class ISpriteHandler;
class Rect;
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
        ~Sprite();

        void setInterface(ISpriteHandler *newInterface);

        std::shared_ptr<Sprite> clone();

        bool isClone() const;

        Sprite *cloneRoot() const;
        Sprite *cloneParent() const;
        const std::vector<std::shared_ptr<Sprite>> &children() const;
        std::vector<std::shared_ptr<Sprite>> allChildren() const;

        bool visible() const;
        void setVisible(bool newVisible);

        double x() const;
        void setX(double newX);

        double y() const;
        void setY(double newY);

        double size() const;
        void setSize(double newSize);

        void setCurrentCostume(int newCostume);

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

    private:
        Target *dataSource() const override;

        spimpl::unique_impl_ptr<SpritePrivate> impl;
};

} // namespace libscratchcpp
