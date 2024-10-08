// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <memory>

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

        virtual void addFunctionCall(const std::string &functionName, int argCount, bool returns) = 0;
        virtual void addConstValue(const Value &value) = 0;
        virtual void addVariableValue(Variable *variable) = 0;
        virtual void addListContents(List *list) = 0;

        virtual void beginIfStatement() = 0;
        virtual void beginElseBranch() = 0;
        virtual void endIf() = 0;

        virtual void beginRepeatLoop() = 0;
        virtual void endLoop() = 0;

        virtual void yield() = 0;
};

} // namespace libscratchcpp
