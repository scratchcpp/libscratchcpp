#pragma once

#include <scratchcpp/target.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class TargetMock : public Target
{
    public:
        MOCK_METHOD(Target *, dataSource, (), (const override));

        Target *dataSourceBase() const { return Target::dataSource(); };
};
