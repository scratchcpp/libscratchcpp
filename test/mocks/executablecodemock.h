#pragma once

#include <scratchcpp/executablecode.h>
#include <scratchcpp/valuedata.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class ExecutableCodeMock : public ExecutableCode
{
    public:
        MOCK_METHOD(void, run, (ExecutionContext *), (override));
        MOCK_METHOD(ValueData, runReporter, (ExecutionContext *), (override));
        MOCK_METHOD(bool, runPredicate, (ExecutionContext *), (override));
        MOCK_METHOD(void, kill, (ExecutionContext *), (override));
        MOCK_METHOD(void, reset, (ExecutionContext *), (override));

        MOCK_METHOD(bool, isFinished, (ExecutionContext *), (const, override));

        MOCK_METHOD(std::shared_ptr<ExecutionContext>, createExecutionContext, (Thread *), (const, override));
};
