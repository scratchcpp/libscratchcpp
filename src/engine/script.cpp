// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/script.h>
#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iengine.h>
#include <iostream>

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

/*! Starts the script (creates a virtual machine) as the given target (might be a clone). */
std::shared_ptr<VirtualMachine> Script::start(Target *target)
{
    auto vm = std::make_shared<VirtualMachine>(target, impl->engine, this);
    vm->setBytecode(impl->bytecode);
    vm->setProcedures(impl->procedures);
    vm->setFunctions(impl->functions);
    vm->setConstValues(impl->constValues);

    Sprite *sprite = nullptr;
    if (target && !target->isStage())
        sprite = dynamic_cast<Sprite *>(target);

    if (impl->target && sprite && sprite->isClone() && impl->engine) {
        Target *root = sprite->cloneSprite();

        if (root != impl->target) {
            std::cout << "warning: a clone tried to start a script of another target (this is a bug in libscratchcpp or in your code!)" << std::endl;
            vm->setVariables(impl->variableValues.data());
            vm->setLists(impl->lists.data());
            return vm;
        }

        // Use internal variables and lists from the clone
        std::vector<Value *> variables;
        std::vector<List *> lists;

        for (const auto &var : impl->variables) {
            Target *owner = var->target();

            if (owner && (owner == root)) {
                auto cloneVar = sprite->variableAt(sprite->findVariableById(var->id()));
                assert(cloneVar);

                if (cloneVar)
                    variables.push_back(cloneVar->valuePtr());
            } else
                variables.push_back(var->valuePtr());
        }

        for (const auto &list : impl->lists) {
            Target *owner = list->target();

            if (owner && (owner == root)) {
                auto cloneList = sprite->listAt(sprite->findListById(list->id()));
                assert(cloneList);

                if (cloneList)
                    lists.push_back(cloneList.get());
            } else
                lists.push_back(list);
        }

        vm->setVariablesVector(variables);
        vm->setListsVector(lists);
    } else {
        vm->setVariables(impl->variableValues.data());
        vm->setLists(impl->lists.data());
    }

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
void Script::setVariables(const std::vector<Variable *> &variables)
{
    impl->variables = variables;
    impl->variableValues.clear();

    for (const auto &var : variables)
        impl->variableValues.push_back(var->valuePtr());
}

/*! Sets the list of lists. */
void Script::setLists(const std::vector<List *> &lists)
{
    impl->lists = lists;
}
