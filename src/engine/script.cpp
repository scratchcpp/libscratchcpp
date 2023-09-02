// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/script.h>
#include <scratchcpp/virtualmachine.h>

#include "script_p.h"

using namespace libscratchcpp;

/*! Constructs Script. */
Script::Script(Target *target, IEngine *engine) :
    impl(spimpl::make_unique_impl<ScriptPrivate>(target, engine))
{
}

/*! Returns the Target. */
Target *Script::target() const
{
    return impl->target;
}

/*! Returns the bytecode array. */
unsigned int *Script::bytecode() const
{
    return impl->bytecode;
}

/*! Returns the bytecode vector. */
const std::vector<unsigned int> &Script::bytecodeVector() const
{
    return impl->bytecodeVector;
}

/*! Sets the bytecode of the script. */
void Script::setBytecode(const std::vector<unsigned int> &code)
{
    impl->bytecodeVector = code;
    impl->bytecode = impl->bytecodeVector.data();
}

/*! Starts the script (creates a virtual machine). */
std::shared_ptr<VirtualMachine> Script::start()
{
    return start(impl->target);
}

/*! Starts the script (creates a virtual machine). */
std::shared_ptr<VirtualMachine> Script::start(Target *target)
{
    auto vm = std::make_shared<VirtualMachine>(target, impl->engine, this);
    vm->setBytecode(impl->bytecode);
    vm->setProcedures(impl->procedures);
    vm->setFunctions(impl->functions);
    vm->setConstValues(impl->constValues);
    vm->setVariables(impl->variables);
    vm->setLists(impl->lists);
    return vm;
}

/*! Sets the list of procedures (custom blocks). */
void Script::setProcedures(const std::vector<unsigned int *> &procedures)
{
    impl->proceduresVector = procedures;
    impl->procedures = impl->proceduresVector.data();
}

/*! Sets the list of functions. */
void Script::setFunctions(const std::vector<BlockFunc> &functions)
{
    impl->functionsVector = functions;
    impl->functions = impl->functionsVector.data();
}

/*! Sets the list of constant values. */
void Script::setConstValues(const std::vector<Value> &values)
{
    impl->constValuesVector = values;
    impl->constValues = impl->constValuesVector.data();
}

/*! Sets the list of variables. */
void Script::setVariables(const std::vector<Value *> &variables)
{
    impl->variablesVector = variables;
    impl->variables = impl->variablesVector.data();
}

/*! Sets the list of lists. */
void Script::setLists(const std::vector<List *> &lists)
{
    impl->listsVector = lists;
    impl->lists = impl->listsVector.data();
}
