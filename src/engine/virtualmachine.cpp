// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/virtualmachine.h>
#include <cassert>

#include "virtualmachine_p.h"

using namespace libscratchcpp;
using namespace vm;

/*! Constructs VirtualMachine. */
VirtualMachine::VirtualMachine() :
    VirtualMachine(nullptr, nullptr, nullptr)
{
}

/*! \copydoc VirtualMachine() */
VirtualMachine::VirtualMachine(Target *target, IEngine *engine, Script *script) :
    impl(spimpl::make_unique_impl<VirtualMachinePrivate>(this, target, engine, script))
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
    unsigned int *ret = impl->run(impl->pos);
    assert(ret);
    if (impl->savePos)
        impl->pos = ret;
}

/*! Jumps back to the initial position. */
void VirtualMachine::reset()
{
    assert(impl->bytecode);
    impl->pos = impl->bytecode;
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
 * \note Nothing will happen if the script is set to run without screen refresh.
 */
void VirtualMachine::stop(bool savePos, bool breakAtomic, bool goBack)
{
    if (impl->warp)
        return;
    impl->stop = true;
    impl->savePos = savePos;
    impl->atomic = !breakAtomic;
    impl->goBack = goBack;
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
