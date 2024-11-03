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

        Compiler(IEngine *engine, Target *target);
        Compiler(const Compiler &) = delete;

        IEngine *engine() const;
        Target *target() const;
        std::shared_ptr<Block> block() const;

        std::shared_ptr<ExecutableCode> compile(std::shared_ptr<Block> startBlock);

        void addFunctionCall(const std::string &functionName, StaticType returnType = StaticType::Void, const std::vector<StaticType> &argTypes = {});
        void addConstValue(const Value &value);
        void addVariableValue(Variable *variable);
        void addListContents(List *list);
        void addInput(const std::string &name);

        void createAdd();
        void createSub();
        void createMul();
        void createDiv();

        void createCmpEQ();
        void createCmpGT();
        void createCmpLT();

        void createAnd();
        void createOr();
        void createNot();

        void moveToIf(std::shared_ptr<Block> substack);
        void moveToIfElse(std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2);
        void moveToRepeatLoop(std::shared_ptr<Block> substack);
        void moveToWhileLoop(std::shared_ptr<Block> substack);
        void moveToRepeatUntilLoop(std::shared_ptr<Block> substack);
        void beginLoopCondition();
        void warp();

        Input *input(const std::string &name) const;
        Field *field(const std::string &name) const;

        const std::unordered_set<std::string> &unsupportedBlocks() const;

    private:
        void addInput(Input *input);

        spimpl::unique_impl_ptr<CompilerPrivate> impl;
};

} // namespace libscratchcpp
