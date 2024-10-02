#pragma once

#include <scratchcpp/dev/executablecode.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ExecutableCodeMock : public ExecutableCode
{
    public:
        MOCK_METHOD(void, run, (ExecutionContext *), (override));
        MOCK_METHOD(void, kill, (ExecutionContext *), (override));
        MOCK_METHOD(void, reset, (ExecutionContext *), (override));

        MOCK_METHOD(bool, isFinished, (ExecutionContext *), (const, override));

        MOCK_METHOD(void, promise, (), (override));
        MOCK_METHOD(void, resolvePromise, (), (override));

        MOCK_METHOD(std::shared_ptr<ExecutionContext>, createExecutionContext, (Target *), (const, override));
};
