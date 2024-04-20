#pragma once

#include <scratchcpp/istagehandler.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class StageHandlerMock : public IStageHandler
{
    public:
        MOCK_METHOD(void, init, (Stage *), (override));

        MOCK_METHOD(void, onCostumeChanged, (Costume *), (override));

        MOCK_METHOD(void, onTempoChanged, (int), (override));
        MOCK_METHOD(void, onVideoStateChanged, (Stage::VideoState), (override));
        MOCK_METHOD(void, onVideoTransparencyChanged, (int), (override));
        MOCK_METHOD(void, onGraphicsEffectChanged, (IGraphicsEffect *, double), (override));
        MOCK_METHOD(void, onGraphicsEffectsCleared, (), (override));
        MOCK_METHOD(void, onBubbleTypeChanged, (Target::BubbleType), (override));
        MOCK_METHOD(void, onBubbleTextChanged, (const std::string &), (override));

        MOCK_METHOD(int, costumeWidth, (), (const, override));
        MOCK_METHOD(int, costumeHeight, (), (const, override));

        MOCK_METHOD(Rect, boundingRect, (), (const, override));
        MOCK_METHOD(Rect, fastBoundingRect, (), (const, override));

        MOCK_METHOD(bool, touchingClones, (const std::vector<Sprite *> &), (const, override));
        MOCK_METHOD(bool, touchingPoint, (double, double), (const, override));
        MOCK_METHOD(bool, touchingColor, (const Value &), (const, override));
};
