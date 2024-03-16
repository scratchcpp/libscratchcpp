// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>
#include <scratchcpp/value.h>

namespace libscratchcpp
{

class Target;
class Block;
class IEngine;
class Variable;
class List;

struct ScriptPrivate
{
        ScriptPrivate(Target *target, std::shared_ptr<Block> topBlock, IEngine *engine);
        ScriptPrivate(const ScriptPrivate &) = delete;

        unsigned int *bytecode = nullptr;
        std::vector<unsigned int> bytecodeVector;

        std::vector<unsigned int> hatPredicateBytecodeVector;
        std::shared_ptr<VirtualMachine> hatPredicateVm;

        Target *target = nullptr;
        std::shared_ptr<Block> topBlock;
        IEngine *engine = nullptr;

        unsigned int **procedures = nullptr;
        std::vector<unsigned int *> proceduresVector;

        BlockFunc *functions = nullptr;
        std::vector<BlockFunc> functionsVector;

        const Value *constValues = nullptr;
        std::vector<Value> constValuesVector;

        std::vector<Value *> variableValues;
        std::vector<Variable *> variables;

        std::vector<List *> lists;
};

} // namespace libscratchcpp
