#pragma once

#include <engine/internal/icodebuilderfactory.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class CodeBuilderFactoryMock : public ICodeBuilderFactory
{
    public:
        MOCK_METHOD(std::shared_ptr<ICodeBuilder>, create, (CompilerContext *, BlockPrototype *, bool), (const, override));
        MOCK_METHOD(std::shared_ptr<CompilerContext>, createCtx, (IEngine *, Target *), (const, override));
};
