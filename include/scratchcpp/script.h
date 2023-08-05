// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class Target;
class IEngine;
class Value;
class VirtualMachine;
class List;
class ScriptPrivate;

/*! \brief The Script class represents a compiled Scratch script. */
class LIBSCRATCHCPP_EXPORT Script
{
    public:
        Script(Target *target, IEngine *engine);
        Script(const Script &) = delete;

        unsigned int *bytecode() const;
        const std::vector<unsigned int> &bytecodeVector() const;
        void setBytecode(const std::vector<unsigned int> &code);

        void setProcedures(const std::vector<unsigned int *> &procedures);
        void setFunctions(const std::vector<BlockFunc> &functions);
        void setConstValues(const std::vector<Value> &values);
        void setVariables(const std::vector<Value *> &variables);
        void setLists(const std::vector<List *> &lists);

        std::shared_ptr<VirtualMachine> start();

    private:
        spimpl::unique_impl_ptr<ScriptPrivate> impl;
};

} // namespace libscratchcpp
