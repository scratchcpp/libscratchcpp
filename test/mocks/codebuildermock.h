#pragma once

#include <dev/engine/internal/icodebuilder.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class CodeBuilderMock : public ICodeBuilder
{
    public:
        MOCK_METHOD(std::shared_ptr<ExecutableCode>, finalize, (), (override));
        MOCK_METHOD(void, addFunctionCall, (const std::string &, Compiler::StaticType, const std::vector<Compiler::StaticType> &), (override));
        MOCK_METHOD(void, addConstValue, (const Value &), (override));
        MOCK_METHOD(void, addVariableValue, (Variable *), (override));
        MOCK_METHOD(void, addListContents, (List *), (override));
        MOCK_METHOD(void, addListItem, (List *), (override));
        MOCK_METHOD(void, addListItemIndex, (List *), (override));
        MOCK_METHOD(void, addListContains, (List *), (override));
        MOCK_METHOD(void, addListSize, (List *), (override));

        MOCK_METHOD(void, createAdd, (), (override));
        MOCK_METHOD(void, createSub, (), (override));
        MOCK_METHOD(void, createMul, (), (override));
        MOCK_METHOD(void, createDiv, (), (override));

        MOCK_METHOD(void, createCmpEQ, (), (override));
        MOCK_METHOD(void, createCmpGT, (), (override));
        MOCK_METHOD(void, createCmpLT, (), (override));

        MOCK_METHOD(void, createAnd, (), (override));
        MOCK_METHOD(void, createOr, (), (override));
        MOCK_METHOD(void, createNot, (), (override));

        MOCK_METHOD(void, createMod, (), (override));
        MOCK_METHOD(void, createRound, (), (override));
        MOCK_METHOD(void, createAbs, (), (override));
        MOCK_METHOD(void, createFloor, (), (override));
        MOCK_METHOD(void, createCeil, (), (override));
        MOCK_METHOD(void, createSqrt, (), (override));
        MOCK_METHOD(void, createSin, (), (override));
        MOCK_METHOD(void, createCos, (), (override));
        MOCK_METHOD(void, createTan, (), (override));
        MOCK_METHOD(void, createAsin, (), (override));
        MOCK_METHOD(void, createAcos, (), (override));
        MOCK_METHOD(void, createAtan, (), (override));
        MOCK_METHOD(void, createLn, (), (override));
        MOCK_METHOD(void, createLog10, (), (override));
        MOCK_METHOD(void, createExp, (), (override));
        MOCK_METHOD(void, createExp10, (), (override));

        MOCK_METHOD(void, createVariableWrite, (Variable *), (override));

        MOCK_METHOD(void, createListClear, (List *), (override));
        MOCK_METHOD(void, createListRemove, (List *), (override));
        MOCK_METHOD(void, createListAppend, (List *), (override));
        MOCK_METHOD(void, createListInsert, (List *), (override));
        MOCK_METHOD(void, createListReplace, (List *), (override));

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
