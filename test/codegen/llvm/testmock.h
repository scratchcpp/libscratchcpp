#pragma once

#include <scratchcpp/valuedata.h>
#include <gmock/gmock.h>

namespace libscratchcpp
{

class Target;
class List;

class TestMock
{
    public:
        MOCK_METHOD(void, script, (ExecutionContext * ctx, Target *, ValueData **, List **));
        MOCK_METHOD(ValueData, reporter, (ExecutionContext * ctx, Target *, ValueData **, List **));
        MOCK_METHOD(bool, predicate, (ExecutionContext * ctx, Target *, ValueData **, List **));
};

} // namespace libscratchcpp
