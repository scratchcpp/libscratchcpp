// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class Target;
class Block;
class IEngine;
class Value;
class VirtualMachine;
class Variable;
class List;
class ScriptPrivate;

/*! \brief The Script class represents a compiled Scratch script. */
class LIBSCRATCHCPP_EXPORT Script
{
    public:
        Script(Target *target, std::shared_ptr<Block> topBlock, IEngine *engine);
        Script(const Script &) = delete;

        Target *target() const;
        std::shared_ptr<Block> topBlock() const;

        unsigned int *bytecode() const;
        const std::vector<unsigned int> &bytecodeVector() const;
        void setBytecode(const std::vector<unsigned int> &code);

        void setHatPredicateBytecode(const std::vector<unsigned int> &code);
        bool runHatPredicate();

        void setProcedures(const std::vector<unsigned int *> &procedures);
        void setFunctions(const std::vector<BlockFunc> &functions);
        void setConstValues(const std::vector<Value> &values);
        void setVariables(const std::vector<Variable *> &variables);
        void setLists(const std::vector<List *> &lists);

        std::shared_ptr<VirtualMachine> start();
        std::shared_ptr<VirtualMachine> start(Target *target);

    private:
        spimpl::unique_impl_ptr<ScriptPrivate> impl;
};

} // namespace libscratchcpp
