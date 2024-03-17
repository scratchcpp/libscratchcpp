// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/script.h>
#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/stage.h>
#include <iostream>

#include "script_p.h"

using namespace libscratchcpp;

/*! Constructs Script. */
Script::Script(Target *target, std::shared_ptr<Block> topBlock, IEngine *engine) :
    impl(spimpl::make_unique_impl<ScriptPrivate>(target, topBlock, engine))
{
}

/*! Returns the Target. */
Target *Script::target() const
{
    return impl->target;
}

std::shared_ptr<Block> Script::topBlock() const
{
    return impl->topBlock;
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

/*! Sets the edge-activated hat predicate bytecode. */
void Script::setHatPredicateBytecode(const std::vector<unsigned int> &code)
{
    impl->hatPredicateBytecodeVector = code;

    if (impl->engine && !code.empty()) {
        impl->hatPredicateVm = std::make_shared<VirtualMachine>(impl->engine->stage(), impl->engine, this);
        impl->hatPredicateVm->setBytecode(impl->hatPredicateBytecodeVector.data());
        impl->hatPredicateVm->setConstValues(impl->constValues);
    }
}

/*!
 * Runs the edge-activated hat predicate and returns the reported value.
 * \note If there isn't any predicate, nothing will happen and the returned value will be false.
 */
bool Script::runHatPredicate()
{
    if (impl->hatPredicateVm && impl->hatPredicateVm->bytecode()) {
        impl->hatPredicateVm->reset();
        impl->hatPredicateVm->setFunctions(getFunctions());
        impl->hatPredicateVm->run();
        assert(impl->hatPredicateVm->registerCount() == 1);

        if (impl->hatPredicateVm->registerCount() == 1)
            return impl->hatPredicateVm->getInput(0, 1)->toBool();
    }

    return false;
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
    vm->setFunctions(getFunctions());
    vm->setConstValues(impl->constValues);

    Sprite *sprite = nullptr;
    if (target && !target->isStage())
        sprite = dynamic_cast<Sprite *>(target);

    if (impl->target && sprite && sprite->isClone()) {
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

/*! Sets the list of constant values. */
void Script::setConstValues(const std::vector<Value> &values)
{
    impl->constValuesVector = values;
    impl->constValues = impl->constValuesVector.data();

    if (impl->hatPredicateVm)
        impl->hatPredicateVm->setConstValues(impl->constValues);
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

BlockFunc *Script::getFunctions() const
{
    if (impl->engine)
        return const_cast<BlockFunc *>(impl->engine->blockFunctions().data());

    return nullptr;
}
