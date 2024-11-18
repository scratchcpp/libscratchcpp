// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/dev/compiler.h>

namespace libscratchcpp
{

class Value;
class Variable;
class List;
class ExecutableCode;

class ICodeBuilder
{
    public:
        virtual ~ICodeBuilder() { }

        virtual std::shared_ptr<ExecutableCode> finalize() = 0;

        virtual void addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const std::vector<Compiler::StaticType> &argTypes) = 0;
        virtual void addConstValue(const Value &value) = 0;
        virtual void addVariableValue(Variable *variable) = 0;
        virtual void addListContents(List *list) = 0;
        virtual void addListItem(List *list) = 0;
        virtual void addListSize(List *list) = 0;

        virtual void createAdd() = 0;
        virtual void createSub() = 0;
        virtual void createMul() = 0;
        virtual void createDiv() = 0;

        virtual void createCmpEQ() = 0;
        virtual void createCmpGT() = 0;
        virtual void createCmpLT() = 0;

        virtual void createAnd() = 0;
        virtual void createOr() = 0;
        virtual void createNot() = 0;

        virtual void createMod() = 0;
        virtual void createRound() = 0;
        virtual void createAbs() = 0;
        virtual void createFloor() = 0;
        virtual void createCeil() = 0;
        virtual void createSqrt() = 0;
        virtual void createSin() = 0;
        virtual void createCos() = 0;
        virtual void createTan() = 0;
        virtual void createAsin() = 0;
        virtual void createAcos() = 0;
        virtual void createAtan() = 0;
        virtual void createLn() = 0;
        virtual void createLog10() = 0;
        virtual void createExp() = 0;
        virtual void createExp10() = 0;

        virtual void createVariableWrite(Variable *variable) = 0;

        virtual void createListClear(List *list) = 0;
        virtual void createListRemove(List *list) = 0;
        virtual void createListAppend(List *list) = 0;
        virtual void createListInsert(List *list) = 0;
        virtual void createListReplace(List *list) = 0;

        virtual void beginIfStatement() = 0;
        virtual void beginElseBranch() = 0;
        virtual void endIf() = 0;

        virtual void beginRepeatLoop() = 0;
        virtual void beginWhileLoop() = 0;
        virtual void beginRepeatUntilLoop() = 0;
        virtual void beginLoopCondition() = 0;
        virtual void endLoop() = 0;

        virtual void yield() = 0;
};

} // namespace libscratchcpp
