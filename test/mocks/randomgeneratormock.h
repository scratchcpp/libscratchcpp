#pragma once

#include <engine/internal/irandomgenerator.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class RandomGeneratorMock : public IRandomGenerator
{
    public:
        MOCK_METHOD(long, randint, (long, long), (const, override));
        MOCK_METHOD(double, randintDouble, (double, double), (const, override));
        MOCK_METHOD(long, randintExcept, (long, long, long), (const, override));
};
