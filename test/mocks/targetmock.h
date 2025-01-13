#pragma once

#include <scratchcpp/target.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class TargetMock : public Target
{
    public:
        MOCK_METHOD(bool, isStage, (), (const, override));

        MOCK_METHOD(void, setCostumeIndex, (int), (override));

        MOCK_METHOD(int, currentCostumeWidth, (), (const, override));
        MOCK_METHOD(int, currentCostumeHeight, (), (const, override));

        MOCK_METHOD(void, setLayerOrder, (int), (override));

        MOCK_METHOD(double, soundEffectValue, (Sound::Effect), (const, override));
        MOCK_METHOD(void, setSoundEffectValue, (Sound::Effect, double), (override));

        MOCK_METHOD(void, clearSoundEffects, (), (override));

        MOCK_METHOD(Rect, boundingRect, (), (const, override));
        MOCK_METHOD(Rect, fastBoundingRect, (), (const, override));

        MOCK_METHOD(bool, touchingPoint, (double, double), (const, override));

        MOCK_METHOD(bool, touchingColor, (Rgb), (const, override));
        MOCK_METHOD(bool, touchingColor, (Rgb, Rgb), (const, override));

        MOCK_METHOD(void, setGraphicsEffectValue, (IGraphicsEffect *, double), (override));
        MOCK_METHOD(void, clearGraphicsEffects, (), (override));

        MOCK_METHOD(Target *, dataSource, (), (const, override));

        Target *dataSourceBase() const { return Target::dataSource(); };

        MOCK_METHOD(bool, touchingClones, (const std::vector<Sprite *> &), (const, override));
};
