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

        virtual CompilerValue *addFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args) = 0;
        virtual CompilerValue *addTargetFunctionCall(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args) = 0;
        virtual CompilerValue *addFunctionCallWithCtx(const std::string &functionName, Compiler::StaticType returnType, const Compiler::ArgTypes &argTypes, const Compiler::Args &args) = 0;
        virtual CompilerConstant *addConstValue(const Value &value) = 0;
        virtual CompilerValue *addLoopIndex() = 0;
        virtual CompilerValue *addVariableValue(Variable *variable) = 0;
        virtual CompilerValue *addListContents(List *list) = 0;
        virtual CompilerValue *addListItem(List *list, CompilerValue *index) = 0;
        virtual CompilerValue *addListItemIndex(List *list, CompilerValue *item) = 0;
        virtual CompilerValue *addListContains(List *list, CompilerValue *item) = 0;
        virtual CompilerValue *addListSize(List *list) = 0;

        virtual CompilerValue *createAdd(CompilerValue *operand1, CompilerValue *operand2) = 0;
        virtual CompilerValue *createSub(CompilerValue *operand1, CompilerValue *operand2) = 0;
        virtual CompilerValue *createMul(CompilerValue *operand1, CompilerValue *operand2) = 0;
        virtual CompilerValue *createDiv(CompilerValue *operand1, CompilerValue *operand2) = 0;

        virtual CompilerValue *createRandom(CompilerValue *from, CompilerValue *to) = 0;

        virtual CompilerValue *createCmpEQ(CompilerValue *operand1, CompilerValue *operand2) = 0;
        virtual CompilerValue *createCmpGT(CompilerValue *operand1, CompilerValue *operand2) = 0;
        virtual CompilerValue *createCmpLT(CompilerValue *operand1, CompilerValue *operand2) = 0;

        virtual CompilerValue *createAnd(CompilerValue *operand1, CompilerValue *operand2) = 0;
        virtual CompilerValue *createOr(CompilerValue *operand1, CompilerValue *operand2) = 0;
        virtual CompilerValue *createNot(CompilerValue *operand) = 0;

        virtual CompilerValue *createMod(CompilerValue *num1, CompilerValue *num2) = 0;
        virtual CompilerValue *createRound(CompilerValue *num) = 0;
        virtual CompilerValue *createAbs(CompilerValue *num) = 0;
        virtual CompilerValue *createFloor(CompilerValue *num) = 0;
        virtual CompilerValue *createCeil(CompilerValue *num) = 0;
        virtual CompilerValue *createSqrt(CompilerValue *num) = 0;
        virtual CompilerValue *createSin(CompilerValue *num) = 0;
        virtual CompilerValue *createCos(CompilerValue *num) = 0;
        virtual CompilerValue *createTan(CompilerValue *num) = 0;
        virtual CompilerValue *createAsin(CompilerValue *num) = 0;
        virtual CompilerValue *createAcos(CompilerValue *num) = 0;
        virtual CompilerValue *createAtan(CompilerValue *num) = 0;
        virtual CompilerValue *createLn(CompilerValue *num) = 0;
        virtual CompilerValue *createLog10(CompilerValue *num) = 0;
        virtual CompilerValue *createExp(CompilerValue *num) = 0;
        virtual CompilerValue *createExp10(CompilerValue *num) = 0;

        virtual CompilerValue *createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType) = 0;

        virtual void createVariableWrite(Variable *variable, CompilerValue *value) = 0;

        virtual void createListClear(List *list) = 0;
        virtual void createListRemove(List *list, CompilerValue *index) = 0;
        virtual void createListAppend(List *list, CompilerValue *item) = 0;
        virtual void createListInsert(List *list, CompilerValue *index, CompilerValue *item) = 0;
        virtual void createListReplace(List *list, CompilerValue *index, CompilerValue *item) = 0;

        virtual void beginIfStatement(CompilerValue *cond) = 0;
        virtual void beginElseBranch() = 0;
        virtual void endIf() = 0;

        virtual void beginRepeatLoop(CompilerValue *count) = 0;
        virtual void beginWhileLoop(CompilerValue *cond) = 0;
        virtual void beginRepeatUntilLoop(CompilerValue *cond) = 0;
        virtual void beginLoopCondition() = 0;
        virtual void endLoop() = 0;

        virtual void yield() = 0;

        virtual void createStop() = 0;
};

} // namespace libscratchcpp
