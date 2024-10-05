#pragma once

#include <dev/engine/internal/icodebuilderfactory.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class CodeBuilderFactoryMock : public ICodeBuilderFactory
{
    public:
        MOCK_METHOD(std::shared_ptr<ICodeBuilder>, create, (const std::string &), (const, override));
};
