// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_set>
#include <vector>

#include "global.h"
#include "enum_bitmask.h"
#include "spimpl.h"

#define BLOCK_EXPORT extern "C" LIBSCRATCHCPP_EXPORT

namespace libscratchcpp
{

class CompilerContext;
class IEngine;
class Target;
class ExecutableCode;
class CompilerValue;
class CompilerConstant;
class CompilerLocalVariable;
class Variable;
class List;
class Input;
class Field;
class BlockPrototype;
class CompilerPrivate;

/*! \brief The Compiler class provides API for compiling Scratch scripts. */
class LIBSCRATCHCPP_EXPORT Compiler
{
    public:
        enum class StaticType
        {
            Void = 0,
            Number = 1 << 0,
            Bool = 1 << 1,
            String = 1 << 2,
            Pointer = 1 << 3,
            Unknown = Number | Bool | String
        };

        enum class CodeType
        {
            Script,
            Reporter,
            HatPredicate
        };

        using ArgTypes = std::vector<StaticType>;
        using Args = std::vector<CompilerValue *>;

        Compiler(CompilerContext *ctx);
        Compiler(IEngine *engine, Target *target);
        Compiler(const Compiler &) = delete;

        IEngine *engine() const;
        Target *target() const;
        Block *block() const;

        std::shared_ptr<ExecutableCode> compile(Block *startBlock, CodeType codeType = CodeType::Script);
        void preoptimize();

        CompilerValue *addFunctionCall(const std::string &functionName, StaticType returnType = StaticType::Void, const ArgTypes &argTypes = {}, const Args &args = {});
        CompilerValue *addTargetFunctionCall(const std::string &functionName, StaticType returnType = StaticType::Void, const ArgTypes &argTypes = {}, const Args &args = {});
        CompilerValue *addFunctionCallWithCtx(const std::string &functionName, StaticType returnType = StaticType::Void, const ArgTypes &argTypes = {}, const Args &args = {});
        CompilerConstant *addConstValue(const Value &value);
        CompilerValue *addStringChar(CompilerValue *string, CompilerValue *index);
        CompilerValue *addStringLength(CompilerValue *string);
        CompilerValue *addLoopIndex();
        CompilerValue *addLocalVariableValue(CompilerLocalVariable *variable);
        CompilerValue *addVariableValue(Variable *variable);
        CompilerValue *addListContents(List *list);
        CompilerValue *addListItem(List *list, CompilerValue *index);
        CompilerValue *addListItemIndex(List *list, CompilerValue *item);
        CompilerValue *addListContains(List *list, CompilerValue *item);
        CompilerValue *addListSize(List *list);
        CompilerValue *addProcedureArgument(const std::string &name);

        CompilerValue *addInput(const std::string &name);
        CompilerValue *addInput(Input *input);

        CompilerValue *createAdd(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createSub(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createMul(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createDiv(CompilerValue *operand1, CompilerValue *operand2);

        CompilerValue *createRandom(CompilerValue *from, CompilerValue *to);
        CompilerValue *createRandomInt(CompilerValue *from, CompilerValue *to);

        CompilerValue *createCmpEQ(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createCmpGT(CompilerValue *operand1, CompilerValue *operand2);
        CompilerValue *createCmpLT(CompilerValue *operand1, CompilerValue *operand2);

        CompilerValue *createStrCmpEQ(CompilerValue *string1, CompilerValue *string2, bool caseSensitive = false);

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

        CompilerValue *createStringConcat(CompilerValue *string1, CompilerValue *string2);

        CompilerValue *createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, Compiler::StaticType valueType);

        CompilerLocalVariable *createLocalVariable(Compiler::StaticType type);
        void createLocalVariableWrite(CompilerLocalVariable *variable, CompilerValue *value);

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

        void moveToIf(CompilerValue *cond, Block *substack);
        void moveToIfElse(CompilerValue *cond, Block *substack1, Block *substack2);
        void moveToRepeatLoop(CompilerValue *count, Block *substack);
        void moveToWhileLoop(CompilerValue *cond, Block *substack);
        void moveToRepeatUntilLoop(CompilerValue *cond, Block *substack);
        void warp();

        void createYield();
        void createStop();
        void createStopWithoutSync();

        void createProcedureCall(BlockPrototype *prototype, const Compiler::Args &args);

        Input *input(const std::string &name) const;
        Field *field(const std::string &name) const;

        const std::unordered_set<std::string> &unsupportedBlocks() const;

        static std::shared_ptr<CompilerContext> createContext(IEngine *engine, Target *target);

    private:
        spimpl::unique_impl_ptr<CompilerPrivate> impl;
};

constexpr bool enable_enum_bitmask(Compiler::StaticType);

} // namespace libscratchcpp
