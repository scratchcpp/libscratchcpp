#pragma once

#include <gmock/gmock.h>

namespace libscratchcpp
{

class Target;

class TestMock
{
    public:
        MOCK_METHOD(void, f, (Target *));
};

} // namespace libscratchcpp
