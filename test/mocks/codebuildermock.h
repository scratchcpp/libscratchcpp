#pragma once

#include <engine/internal/icodebuilder.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

class CodeBuilderMock : public ICodeBuilder
{
    public:
        MOCK_METHOD(std::shared_ptr<ExecutableCode>, build, (), (override));
        MOCK_METHOD(CompilerValue *, addFunctionCall, (const std::string &, Compiler::StaticType, const Compiler::ArgTypes &, const Compiler::Args &), (override));
        MOCK_METHOD(CompilerValue *, addTargetFunctionCall, (const std::string &, Compiler::StaticType, const Compiler::ArgTypes &, const Compiler::Args &), (override));
        MOCK_METHOD(CompilerValue *, addFunctionCallWithCtx, (const std::string &, Compiler::StaticType, const Compiler::ArgTypes &, const Compiler::Args &), (override));
        MOCK_METHOD(CompilerConstant *, addConstValue, (const Value &), (override));
        MOCK_METHOD(CompilerValue *, addStringChar, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, addStringLength, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, addLoopIndex, (), (override));
        MOCK_METHOD(CompilerValue *, addLocalVariableValue, (CompilerLocalVariable *), (override));
        MOCK_METHOD(CompilerValue *, addVariableValue, (Variable *), (override));
        MOCK_METHOD(CompilerValue *, addListContents, (List *), (override));
        MOCK_METHOD(CompilerValue *, addListItem, (List *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, addListItemIndex, (List *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, addListContains, (List *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, addListSize, (List *), (override));
        MOCK_METHOD(CompilerValue *, addProcedureArgument, (const std::string &), (override));

        MOCK_METHOD(CompilerValue *, createAdd, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createSub, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createMul, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createDiv, (CompilerValue *, CompilerValue *), (override));

        MOCK_METHOD(CompilerValue *, createRandom, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createRandomInt, (CompilerValue *, CompilerValue *), (override));

        MOCK_METHOD(CompilerValue *, createCmpEQ, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createCmpGT, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createCmpLT, (CompilerValue *, CompilerValue *), (override));

        MOCK_METHOD(CompilerValue *, createStrCmpEQ, (CompilerValue *, CompilerValue *, bool), (override));

        MOCK_METHOD(CompilerValue *, createAnd, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createOr, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createNot, (CompilerValue *), (override));

        MOCK_METHOD(CompilerValue *, createMod, (CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createRound, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createAbs, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createFloor, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createCeil, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createSqrt, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createSin, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createCos, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createTan, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createAsin, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createAcos, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createAtan, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createLn, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createLog10, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createExp, (CompilerValue *), (override));
        MOCK_METHOD(CompilerValue *, createExp10, (CompilerValue *), (override));

        MOCK_METHOD(CompilerValue *, createStringConcat, (CompilerValue *, CompilerValue *), (override));

        MOCK_METHOD(CompilerValue *, createSelect, (CompilerValue *, CompilerValue *, CompilerValue *, Compiler::StaticType), (override));

        MOCK_METHOD(CompilerLocalVariable *, createLocalVariable, (Compiler::StaticType), (override));
        MOCK_METHOD(void, createLocalVariableWrite, (CompilerLocalVariable *, CompilerValue *), (override));

        MOCK_METHOD(void, createVariableWrite, (Variable *, CompilerValue *), (override));

        MOCK_METHOD(void, createListClear, (List *), (override));
        MOCK_METHOD(void, createListRemove, (List *, CompilerValue *), (override));
        MOCK_METHOD(void, createListAppend, (List *, CompilerValue *), (override));
        MOCK_METHOD(void, createListInsert, (List *, CompilerValue *, CompilerValue *), (override));
        MOCK_METHOD(void, createListReplace, (List *, CompilerValue *, CompilerValue *), (override));

        MOCK_METHOD(void, beginIfStatement, (CompilerValue *), (override));
        MOCK_METHOD(void, beginElseBranch, (), (override));
        MOCK_METHOD(void, endIf, (), (override));

        MOCK_METHOD(void, beginRepeatLoop, (CompilerValue *), (override));
        MOCK_METHOD(void, beginWhileLoop, (CompilerValue *), (override));
        MOCK_METHOD(void, beginRepeatUntilLoop, (CompilerValue *), (override));
        MOCK_METHOD(void, beginLoopCondition, (), (override));
        MOCK_METHOD(void, endLoop, (), (override));

        MOCK_METHOD(void, yield, (), (override));

        MOCK_METHOD(void, createStop, (), (override));
        MOCK_METHOD(void, createStopWithoutSync, (), (override));

        MOCK_METHOD(void, createProcedureCall, (BlockPrototype *, const Compiler::Args &), (override));
};
