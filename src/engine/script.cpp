// SPDX-License-Identifier: Apache-2.0

#include "script.h"

using namespace libscratchcpp;

/*! Constructs Script. */
Script::Script(Target *target, Engine *engine) :
    m_target(target),
    m_engine(engine)
{
}

/*! Returns the bytecode array. */
unsigned int *Script::bytecode() const
{
    return m_bytecode;
}

/*! Returns the bytecode vector. */
const std::vector<unsigned int> &Script::bytecodeVector() const
{
    return m_bytecodeVector;
}

/*! Sets the bytecode of the script. */
void Script::setBytecode(const std::vector<unsigned int> &code)
{
    m_bytecodeVector = code;
    m_bytecode = m_bytecodeVector.data();
}

/*! Starts the script (creates a virtual machine). */
std::shared_ptr<VirtualMachine> Script::start()
{
    auto vm = std::make_shared<VirtualMachine>(m_target, m_engine, this);
    vm->setBytecode(m_bytecode);
    vm->setProcedures(m_procedures);
    vm->setFunctions(m_functions);
    vm->setConstValues(m_constValues);
    vm->setVariables(m_variables);
    vm->setLists(m_lists);
    return vm;
}

/*! Sets the list of procedures (custom blocks). */
void Script::setProcedures(const std::vector<unsigned int *> &procedures)
{
    m_proceduresVector = procedures;
    m_procedures = m_proceduresVector.data();
}

/*! Sets the list of functions. */
void Script::setFunctions(const std::vector<BlockFunc> &functions)
{
    m_functionsVector = functions;
    m_functions = m_functionsVector.data();
}

/*! Sets the list of constant values. */
void Script::setConstValues(const std::vector<Value> &values)
{
    m_constValuesVector = values;
    m_constValues = m_constValuesVector.data();
}

/*! Sets the list of variables. */
void Script::setVariables(const std::vector<Value *> &variables)
{
    m_variablesVector = variables;
    m_variables = m_variablesVector.data();
}

/*! Sets the list of lists. */
void Script::setLists(const std::vector<List *> &lists)
{
    m_listsVector = lists;
    m_lists = m_listsVector.data();
}
