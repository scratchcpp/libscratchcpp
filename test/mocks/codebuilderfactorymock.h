#pragma once

#include <engine/internal/codegen/icodebuilderfactory.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class CodeBuilderFactoryMock : public ICodeBuilderFactory
{
    public:
        MOCK_METHOD(std::shared_ptr<ICodeBuilder>, create, (CompilerContext *, BlockPrototype *, Compiler::CodeType), (const, override));
        MOCK_METHOD(std::shared_ptr<CompilerContext>, createCtx, (IEngine *, Target *, const std::unordered_set<std::string> &), (const, override));
};
