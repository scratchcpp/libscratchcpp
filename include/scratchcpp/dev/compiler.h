// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_set>
#include <vector>

#include "../global.h"
#include "../spimpl.h"

namespace libscratchcpp
{

class IEngine;
class Target;
class ExecutableCode;
class CompilerValue;
class CompilerConstant;
class Variable;
class List;
class Input;
class Field;
class CompilerPrivate;

/*! \brief The Compiler class provides API for compiling Scratch scripts. */
class LIBSCRATCHCPP_EXPORT Compiler
{
    public:
        enum class StaticType
        {
            Void,
            Number,
            Bool,
            String,
            Unknown
        };

        using ArgTypes = std::vector<StaticType>;
        using Args = std::vector<CompilerValue *>;

        Compiler(IEngine *engine, Target *target);
        Compiler(const Compiler &) = delete;

        IEngine *engine() const;
        Target *target() const;
        std::shared_ptr<Block> block() const;

        std::shared_ptr<ExecutableCode> compile(std::shared_ptr<Block> startBlock);

        CompilerValue *addFunctionCall(const std::string &functionName, StaticType returnType = StaticType::Void, const ArgTypes &argTypes = {}, const Args &args = {});
        CompilerValue *addTargetFunctionCall(const std::string &functionName, StaticType returnType = StaticType::Void, const ArgTypes &argTypes = {}, const Args &args = {});
        CompilerValue *addFunctionCallWithCtx(const std::string &functionName, StaticType returnType = StaticType::Void, const ArgTypes &argTypes = {}, const Args &args = {});
        CompilerConstant *addConstValue(const Value &value);
        CompilerValue *addVariableValue(Variable *variable);
        CompilerValue *addListContents(List *list);
        CompilerValue *addListItem(List *list, CompilerValue *index);
        CompilerValue *addListItemIndex(List *list, CompilerValue *item);
        CompilerValue *addListContains(List *list, CompilerValue *item);
        CompilerValue *addListSize(List *list);
        CompilerValue *addInput(const std::string &name);

        CompilerValue *createAdd(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createSub(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createMul(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createDiv(CompilerValue *operand1, CompilerValue *operand2);

        CompilerValue *createCmpEQ(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createCmpGT(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createCmpLT(CompilerValue *operand1, CompilerValue *operand2);

        CompilerValue *createAnd(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createOr(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createNot(CompilerValue *operand);

        CompilerValue *createMod(CompilerValue *num1, CompilerValue *num2);
        CompilerValue *createRound(CompilerValue *num);
        CompilerValue *createAbs(CompilerValue *num);
        CompilerValue *createFloor(CompilerValue *num);
        CompilerValue *createCeil(CompilerValue *num);
        CompilerValue *createSqrt(CompilerValue *num);
        CompilerValue *createSin(CompilerValue *num);
        CompilerValue *createCos(CompilerValue *num);
        CompilerValue *createTan(CompilerValue *num);
        CompilerValue *createAsin(CompilerValue *num);
        CompilerValue *createAcos(CompilerValue *num);
        CompilerValue *createAtan(CompilerValue *num);
        CompilerValue *createLn(CompilerValue *num);
        CompilerValue *createLog10(CompilerValue *num);
        CompilerValue *createExp(CompilerValue *num);
        CompilerValue *createExp10(CompilerValue *num);

        CompilerValue *createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType);

        void createVariableWrite(Variable *variable, CompilerValue *value);

        void createListClear(List *list);
        void createListRemove(List *list, CompilerValue *index);
        void createListAppend(List *list, CompilerValue *item);
        void createListInsert(List *list, CompilerValue *index, CompilerValue *item);
        void createListReplace(List *list, CompilerValue *index, CompilerValue *item);

        void beginIfStatement(CompilerValue *cond);
        void beginElseBranch();
        void endIf();

        void beginWhileLoop(CompilerValue *cond);
        void beginRepeatUntilLoop(CompilerValue *cond);
        void beginLoopCondition();
        void endLoop();

        void moveToIf(CompilerValue *cond, std::shared_ptr<Block> substack);
        void moveToIfElse(CompilerValue *cond, std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2);
        void moveToRepeatLoop(CompilerValue *count, std::shared_ptr<Block> substack);
        void moveToWhileLoop(CompilerValue *cond, std::shared_ptr<Block> substack);
        void moveToRepeatUntilLoop(CompilerValue *cond, std::shared_ptr<Block> substack);
        void warp();

        void createYield();
        void createStop();

        Input *input(const std::string &name) const;
        Field *field(const std::string &name) const;

        const std::unordered_set<std::string> &unsupportedBlocks() const;

    private:
        CompilerValue *addInput(Input *input);

        spimpl::unique_impl_ptr<CompilerPrivate> impl;
};

} // namespace libscratchcpp
