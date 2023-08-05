#pragma once

#include <scratchcpp/ispritehandler.h>

using namespace libscratchcpp;

class TestSpriteInterface : public ISpriteHandler
{
    public:
        TestSpriteInterface();

        void onSpriteChanged(Sprite *sprite) override;

        void onCostumeChanged(const char *data) override;

        void onVisibleChanged(bool visible) override;
        void onXChanged(double x) override;
        void onYChanged(double y) override;
        void onSizeChanged(double size) override;
        void onDirectionChanged(double direction) override;
        void onRotationStyleChanged(Sprite::RotationStyle rotationStyle) override;

        Sprite *sprite = nullptr;

        const char *costumeData = nullptr;

        bool visible = true;
        double x = 0;
        double y = 0;
        double size = 100;
        double direction = 90;
        Sprite::RotationStyle rotationStyle = Sprite::RotationStyle::AllAround;
};
