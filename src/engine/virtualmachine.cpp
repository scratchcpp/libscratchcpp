// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <cassert>

#include "virtualmachine_p.h"

using namespace libscratchcpp;
using namespace vm;

/*! Constructs VirtualMachine. */
VirtualMachine::VirtualMachine() :
    VirtualMachine(nullptr, nullptr, nullptr, nullptr)
{
}

/*! \copydoc VirtualMachine() */
VirtualMachine::VirtualMachine(Target *target, IEngine *engine, Script *script, Thread *thread) :
    impl(spimpl::make_unique_impl<VirtualMachinePrivate>(this, target, engine, script, thread))
{
}

/*! Sets the array of procedures (custom blocks). */
void VirtualMachine::setProcedures(unsigned int **procedures)
{
    impl->procedures = procedures;
}

/*! Sets the list of functions. */
void VirtualMachine::setFunctions(BlockFunc *functions)
{
    impl->functions = functions;
}

/*! Sets the list of constant values. */
void VirtualMachine::setConstValues(const Value *values)
{
    impl->constValues = values;
}

/*! Sets the list of variables. */
void VirtualMachine::setVariables(Value **variables)
{
    impl->variables = variables;
}

/*! Sets the list of lists. */
void VirtualMachine::setLists(List **lists)
{
    impl->lists = lists;
}

/*!
 * Sets the vector of variables.
 * Use this instead of setVariables() if you can't store the array (data pointer) anywhere.
 */
void VirtualMachine::setVariablesVector(const std::vector<Value *> &variables)
{
    impl->variablesVector = variables;
    impl->variables = impl->variablesVector.data();
}

/*!
 * Sets the vector of lists.
 * Use this instead of setLists() if you can't store the array (data pointer) anywhere.
 */
void VirtualMachine::setListsVector(const std::vector<List *> &lists)
{
    impl->listsVector = lists;
    impl->lists = impl->listsVector.data();
}

/*! Sets the bytecode of the script. */
void VirtualMachine::setBytecode(unsigned int *code)
{
    impl->bytecode = code;
    impl->pos = code;
}

/*! Returns the array of procedures. */
unsigned int **VirtualMachine::procedures() const
{
    return impl->procedures;
}

/*! Returns the array of functions (block implementation function pointers). */
const BlockFunc *VirtualMachine::functions() const
{
    return impl->functions;
}

/*! Returns the array of constant values. */
const Value *VirtualMachine::constValues() const
{
    return impl->constValues;
}

/*! Returns the array of Value pointers of variables. */
Value **VirtualMachine::variables() const
{
    return impl->variables;
}

/*! Returns the array of lists. */
List **VirtualMachine::lists() const
{
    return impl->lists;
}

/*! Returns the bytecode array. */
unsigned int *VirtualMachine::bytecode() const
{
    return impl->bytecode;
}

/*! Returns number of currently used registers. */
size_t VirtualMachine::registerCount() const
{
    return impl->regCount;
}

/*! Returns the Target the VM belongs to. */
Target *VirtualMachine::target() const
{
    return impl->target;
}

/*! Returns the engine of current project. */
IEngine *VirtualMachine::engine() const
{
    return impl->engine;
}

/*! Returns the Script that created the VM. */
Script *VirtualMachine::script() const
{
    return impl->script;
}

/*! Returns the Thread this VM belongs to. */
Thread *VirtualMachine::thread() const
{
    return impl->thread;
}

/*! Returns the register at the given index with the given argument (register) count. */
const Value *VirtualMachine::getInput(unsigned int index, unsigned int argCount) const
{
    return impl->regs[impl->regCount - argCount + index];
}

/*! Adds the given value to registers. */
void VirtualMachine::addReturnValue(const Value &v)
{
    *impl->regs[impl->regCount++] = v;
}

/*!
 * Replaces the given register. For example:
 * \code
 * replaceReturnValue("...", 1) // write to last register
 * replaceReturnValue("...", 2) // write to second last register
 * \endcode
 */
void VirtualMachine::replaceReturnValue(const Value &v, unsigned int offset)
{
    *impl->regs[impl->regCount - offset] = v;
}

/*! Continues running the script from last position (the first instruction is skipped). */
void VirtualMachine::run()
{
    impl->running = true;

    if (impl->promisePending)
        return;

    unsigned int *ret = impl->run(impl->pos);
    assert(ret);

    if (impl->savePos)
        impl->pos = ret;

    impl->running = false;
}

/*! Stops the execution and sets atEnd to true. */
void VirtualMachine::kill()
{
    reset();
    impl->atEnd = true;
}

/*! Jumps back to the initial position. */
void VirtualMachine::reset()
{
    impl->pos = impl->bytecode;
    impl->atEnd = false;
    impl->promisePending = false;

    if (!impl->running) // Registers will be freed when the script stops running
        impl->regCount = 0;
}

/*! Moves back to the last vm::OP_CHECKPOINT instruction in the bytecode. */
void VirtualMachine::moveToLastCheckpoint()
{
    assert(impl->checkpoint);
    impl->pos = impl->checkpoint;
    impl->updatePos = true;
}

/*!
 * Use this to stop the script from a function.
 * \param[in] savePos Changes the return value of savePos().
 * \param[in] breakAtomic Whether to break the frame after stopping the script.
 * \param[in] goBack Whether to go back so that the current instruction can run again in the future.
 * \note If the script is set to run without screen refresh, the VM won't stop.
 * The only parameter which won't be ignored is goBack.
 */
void VirtualMachine::stop(bool savePos, bool breakFrame, bool goBack)
{
    impl->stop = true;
    impl->savePos = savePos && !impl->warp;
    impl->noBreak = !breakFrame || impl->warp;
    impl->goBack = goBack;
}

/*!
 * Use this to pause the execution of the script.
 * The execution will continue after calling resolvePromise()
 */
void VirtualMachine::promise()
{
    impl->promisePending = true;
    stop();
}

/*! Resolves the promise so that the VM can continue with the execution. */
void VirtualMachine::resolvePromise()
{
    impl->promisePending = false;
}

/*! Returns true if the VM has reached the vm::OP_HALT instruction. */
bool VirtualMachine::atEnd() const
{
    return impl->atEnd;
}

/*! Used to check whether the position can be preserved. */
bool VirtualMachine::savePos() const
{
    return impl->savePos;
}
