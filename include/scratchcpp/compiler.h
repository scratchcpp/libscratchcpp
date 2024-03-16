// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

#include "global.h"
#include "spimpl.h"
#include "virtualmachine.h"
#include "value.h"

namespace libscratchcpp
{

class IEngine;
class Block;
class Input;
class InputValue;
class Field;
class Variable;
class List;
class BlockPrototype;
class Entity;
class CompilerPrivate;

/*! \brief The Compiler class provides an API for compiling scripts of targets to bytecode. */
class LIBSCRATCHCPP_EXPORT Compiler
{
    public:
        enum class SubstackType
        {
            Loop,
            IfStatement
        };

        Compiler(IEngine *engine, Target *target = nullptr);
        Compiler(const Compiler &) = delete;

        void init();
        void compile(std::shared_ptr<Block> topLevelBlock);
        void end();

        const std::vector<unsigned int> &bytecode() const;
        const std::vector<unsigned int> &hatPredicateBytecode() const;

        IEngine *engine() const;
        Target *target() const;

        const std::vector<InputValue *> &constInputValues() const;
        std::vector<Value> constValues() const;

        const std::vector<Variable *> &variables() const;
        std::vector<Value *> variablePtrs() const;

        const std::vector<List *> &lists() const;

        void addInstruction(vm::Opcode opcode, const std::initializer_list<unsigned int> &args = {});
        void addInput(Input *input);
        void addInput(int id);
        void addConstValue(const Value &value);
        void addFunctionCall(BlockFunc f);
        void addProcedureArg(const std::string &procCode, const std::string &argName);
        void moveToSubstack(std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2, SubstackType type);
        void moveToSubstack(std::shared_ptr<Block> substack, SubstackType type);
        void warp();

        Input *input(int id) const;
        Field *field(int id) const;
        std::shared_ptr<Block> inputBlock(int id) const;
        unsigned int variableIndex(std::shared_ptr<Entity> varEntity);
        unsigned int listIndex(std::shared_ptr<Entity> listEntity);
        unsigned int constIndex(InputValue *value);
        unsigned int procedureIndex(const std::string &proc);
        long procedureArgIndex(const std::string &procCode, const std::string &argName);

        const std::vector<std::string> &procedures() const;

        const std::shared_ptr<Block> &block() const;
        void setBlock(std::shared_ptr<Block> block);

        BlockPrototype *procedurePrototype() const;
        void setProcedurePrototype(BlockPrototype *prototype);

    private:
        spimpl::unique_impl_ptr<CompilerPrivate> impl;
};

} // namespace libscratchcpp
