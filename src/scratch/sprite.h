// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "target.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT IScratchSprite;

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

        void setInterface(IScratchTarget *newInterface) override;

        bool visible() const;
        void setVisible(bool newVisible);

        double x() const;
        void setX(double newX);

        double y() const;
        void setY(double newY);

        double size() const;
        void setSize(double newSize);

        double direction() const;
        void setDirection(double newDirection);

        bool draggable() const;
        void setDraggable(bool newDraggable);

        RotationStyle rotationStyle() const;
        std::string rotationStyleStr() const;
        void setRotationStyle(RotationStyle newRotationStyle);
        void setRotationStyle(const std::string &newRotationStyle);
        void setRotationStyle(const char *newRotationStyle);

    protected:
        void setCostumeData(const char *data) override;

    private:
        IScratchSprite *m_interface = nullptr;
        bool m_visible = true;
        double m_x = 0;
        double m_y = 0;
        double m_size = 100;
        double m_direction = 90;
        bool m_draggable = false;
        RotationStyle m_rotationStyle = RotationStyle::AllAround;
};

} // namespace libscratchcpp
