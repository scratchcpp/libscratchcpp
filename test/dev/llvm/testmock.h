#pragma once

#include <gmock/gmock.h>

namespace libscratchcpp
{

class Target;
struct ValueData;
class List;

class TestMock
{
    public:
        MOCK_METHOD(void, f, (ExecutionContext * ctx, Target *, ValueData **, List **));
};

} // namespace libscratchcpp
