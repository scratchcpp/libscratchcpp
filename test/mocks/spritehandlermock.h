#pragma once

#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/rect.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class SpriteHandlerMock : public ISpriteHandler
{
    public:
        MOCK_METHOD(void, init, (Sprite *), (override));
        MOCK_METHOD(void, deinitClone, (), (override));

        MOCK_METHOD(void, onCloned, (Sprite *), (override));

        MOCK_METHOD(void, onCostumeChanged, (Costume *), (override));

        MOCK_METHOD(void, onVisibleChanged, (bool), (override));
        MOCK_METHOD(void, onXChanged, (double), (override));
        MOCK_METHOD(void, onYChanged, (double), (override));
        MOCK_METHOD(void, onMoved, (double, double, double, double), (override));
        MOCK_METHOD(void, onSizeChanged, (double), (override));
        MOCK_METHOD(void, onDirectionChanged, (double), (override));
        MOCK_METHOD(void, onRotationStyleChanged, (Sprite::RotationStyle), (override));
        MOCK_METHOD(void, onLayerOrderChanged, (int), (override));
        MOCK_METHOD(void, onGraphicsEffectChanged, (IGraphicsEffect *, double), (override));
        MOCK_METHOD(void, onGraphicsEffectsCleared, (), (override));
        MOCK_METHOD(void, onBubbleTypeChanged, (Target::BubbleType), (override));
        MOCK_METHOD(void, onBubbleTextChanged, (const std::string &), (override));

        MOCK_METHOD(Rect, boundingRect, (), (const, override));
        MOCK_METHOD(Rect, fastBoundingRect, (), (const, override));
        MOCK_METHOD(bool, touchingPoint, (double, double), (const, override));
};
