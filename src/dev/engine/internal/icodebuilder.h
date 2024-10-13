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

        virtual void createAdd() = 0;
        virtual void createSub() = 0;
        virtual void createMul() = 0;
        virtual void createDiv() = 0;

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
