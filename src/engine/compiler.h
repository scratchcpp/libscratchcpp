// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "virtualmachine.h"
#include <vector>

namespace libscratchcpp
{

/*! \brief The Compiler class provides an API for compiling Scratch scripts to bytecode. */
class LIBSCRATCHCPP_EXPORT Compiler
{
    public:
        Compiler(Engine *engine, std::shared_ptr<Block> topLevelBlock);
        Compiler(const Compiler &) = delete;

        void compile();

        const std::vector<unsigned int> &bytecode() const;

        const std::vector<InputValue *> &constInputValues() const;
        std::vector<Value> constValues() const;
        void setConstInputValues(const std::vector<InputValue *> &values);

        const std::vector<Variable *> &variables() const;
        std::vector<Value *> variablePtrs() const;
        void setVariables(std::vector<Variable *> variables);

        const std::vector<List *> &lists() const;
        void setLists(std::vector<List *> lists);

        void addInstruction(vm::Opcode opcode, std::initializer_list<unsigned int> args = {});
        void addInput(Input *input);

        Input *input(int id) const;
        Field *field(int id) const;
        unsigned int variableIndex(std::shared_ptr<IEntity> varEntity);

    private:
        unsigned int constIndex(InputValue *value);

        Engine *m_engine;
        std::shared_ptr<Block>(m_block);

        std::vector<unsigned int> m_bytecode;
        std::vector<InputValue *> m_constValues;
        std::vector<Variable *> m_variables;
        std::vector<List *> m_lists;
};

} // namespace libscratchcpp
