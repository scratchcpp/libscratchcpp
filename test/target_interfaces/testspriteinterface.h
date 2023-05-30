#pragma once

#include <iscratchsprite.h>

using namespace libscratchcpp;

class TestSpriteInterface : public IScratchSprite
{
    public:
        TestSpriteInterface();

        void setVisible(bool visible) override;
        void setX(double x) override;
        void setY(double y) override;
        void setSize(double size) override;
        void setDirection(double direction) override;
        void setRotationStyle(Sprite::RotationStyle rotationStyle) override;

        void setCostume(const char *data) override;

        bool visible = true;
        double x = 0;
        double y = 0;
        double size = 100;
        double direction = 90;
        Sprite::RotationStyle rotationStyle = Sprite::RotationStyle::AllAround;

        const char *costumeData = nullptr;
};
