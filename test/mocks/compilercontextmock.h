#pragma once

#include <scratchcpp/compilercontext.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class CompilerContextMock : public CompilerContext
{
    public:
        CompilerContextMock(IEngine *engine, Target *target) :
            CompilerContext(engine, target)
        {
        }

        MOCK_METHOD(void, preoptimize, (), (override));
};
