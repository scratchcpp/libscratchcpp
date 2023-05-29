// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "virtualmachine.h"

namespace libscratchcpp
{

/*! \brief The Script class represents a compiled Scratch script. */
class LIBSCRATCHCPP_EXPORT Script
{
    public:
        Script(Target *target, Engine *engine);
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
        unsigned int *m_bytecode;
        std::vector<unsigned int> m_bytecodeVector;

        Target *m_target;
        Engine *m_engine;

        unsigned int **m_procedures = nullptr;
        std::vector<unsigned int *> m_proceduresVector;

        BlockFunc *m_functions;
        std::vector<BlockFunc> m_functionsVector;

        const Value *m_constValues = nullptr;
        std::vector<Value> m_constValuesVector;

        Value **m_variables = nullptr;
        std::vector<Value *> m_variablesVector;

        List **m_lists = nullptr;
        std::vector<List *> m_listsVector;
};

} // namespace libscratchcpp
