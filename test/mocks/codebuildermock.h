#pragma once

#include <dev/engine/internal/icodebuilder.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class CodeBuilderMock : public ICodeBuilder
{
    public:
        MOCK_METHOD(std::shared_ptr<ExecutableCode>, finalize, (), (override));
        MOCK_METHOD(void, addFunctionCall, (const std::string &, int, bool), (override));
        MOCK_METHOD(void, addConstValue, (const Value &), (override));
        MOCK_METHOD(void, addVariableValue, (Variable *), (override));
        MOCK_METHOD(void, addListContents, (List *), (override));

        MOCK_METHOD(void, beginIfStatement, (), (override));
        MOCK_METHOD(void, beginElseBranch, (), (override));
        MOCK_METHOD(void, endIf, (), (override));

        MOCK_METHOD(void, beginRepeatLoop, (), (override));
        MOCK_METHOD(void, beginWhileLoop, (), (override));
        MOCK_METHOD(void, beginRepeatUntilLoop, (), (override));
        MOCK_METHOD(void, beginLoopCondition, (), (override));
        MOCK_METHOD(void, endLoop, (), (override));

        MOCK_METHOD(void, yield, (), (override));
};
