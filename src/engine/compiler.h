// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "virtualmachine.h"
#include <vector>

namespace libscratchcpp
{

/*! \brief The Compiler class provides an API for compiling scripts of targets to bytecode. */
class LIBSCRATCHCPP_EXPORT Compiler
{
    public:
        enum class SubstackType
        {
            Loop,
            IfStatement
        };

        Compiler(Engine *engine);
        Compiler(const Compiler &) = delete;

        void compile(std::shared_ptr<Block> topLevelBlock);

        const std::vector<unsigned int> &bytecode() const;

        const std::vector<InputValue *> &constInputValues() const;
        std::vector<Value> constValues() const;

        const std::vector<Variable *> &variables() const;
        std::vector<Value *> variablePtrs() const;

        const std::vector<List *> &lists() const;

        void addInstruction(vm::Opcode opcode, std::initializer_list<unsigned int> args = {});
        void addInput(int id);
        void addFunctionCall(BlockFunc f);
        void moveToSubstack(std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2, SubstackType type);
        void moveToSubstack(std::shared_ptr<Block> substack, SubstackType type);

        Input *input(int id) const;
        Field *field(int id) const;
        std::shared_ptr<Block> inputBlock(int id) const;
        unsigned int variableIndex(std::shared_ptr<IEntity> varEntity);
        unsigned int listIndex(std::shared_ptr<IEntity> listEntity);
        unsigned int constIndex(InputValue *value);
        unsigned int procedureIndex(std::string proc);

        const std::vector<std::string> &procedures() const;

        const std::shared_ptr<Block> &block() const;

    private:
        void substackEnd();

        Engine *m_engine;
        std::shared_ptr<Block> m_block;
        std::vector<std::pair<std::pair<std::shared_ptr<Block>, std::shared_ptr<Block>>, SubstackType>> m_substackTree;

        std::vector<unsigned int> m_bytecode;
        std::vector<InputValue *> m_constValues;
        std::vector<Variable *> m_variables;
        std::vector<List *> m_lists;
        std::vector<std::string> m_procedures;
};

} // namespace libscratchcpp
