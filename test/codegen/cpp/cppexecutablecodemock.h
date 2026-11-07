#pragma once

#include <gmock/gmock.h>
#include <scratchcpp/valuedata.h>
#include <engine/internal/codegen/cpp/cppexecutablecode.h>

namespace libscratchcpp
{

class CppExecutableCodeMock : public CppExecutableCode
{
    public:
        CppExecutableCodeMock(CppCompilerContext *ctx) :
            CppExecutableCode(ctx)
        {
        }

        MOCK_METHOD(CppCoroutine, runScript, (ExecutionContext * ctx, Target *, ValueData **, List **));
        MOCK_METHOD(ValueData, runReporter, (ExecutionContext * ctx, Target *, ValueData **, List **));
        MOCK_METHOD(bool, runPredicate, (ExecutionContext * ctx, Target *, ValueData **, List **));
};

} // namespace libscratchcpp
