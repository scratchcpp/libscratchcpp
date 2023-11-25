#pragma once

#include <scratchcpp/ispritehandler.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class SpriteHandlerMock : public ISpriteHandler
{
    public:
        MOCK_METHOD(void, init, (Sprite *), (override));

        MOCK_METHOD(void, onCloned, (Sprite *), (override));

        MOCK_METHOD(void, onCostumeChanged, (Costume *), (override));

        MOCK_METHOD(void, onVisibleChanged, (bool), (override));
        MOCK_METHOD(void, onXChanged, (double), (override));
        MOCK_METHOD(void, onYChanged, (double), (override));
        MOCK_METHOD(void, onSizeChanged, (double), (override));
        MOCK_METHOD(void, onDirectionChanged, (double), (override));
        MOCK_METHOD(void, onRotationStyleChanged, (Sprite::RotationStyle), (override));
};
