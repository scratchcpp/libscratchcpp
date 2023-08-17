// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <scratchcpp/input.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/block.h>

#include "input_p.h"

using namespace libscratchcpp;

/*! Constructs Input. */
Input::Input(const std::string &name, Type type) :
    impl(spimpl::make_unique_impl<InputPrivate>(name, type))
{
}

/*! Returns the name of the input. */
const std::string &Input::name() const
{
    return impl->name;
}

/*! Returns the ID of the input. */
int Input::inputId() const
{
    return impl->inputId;
}

/*! Sets the ID of the input. */
void Input::setInputId(int newInputId)
{
    impl->inputId = newInputId;
}

/*! Returns the type of the input. */
Input::Type Input::type() const
{
    return impl->type;
}

/*! Returns the primary value. */
InputValue *Input::primaryValue()
{
    return &impl->primaryValue;
}

/*! Returns the secondary value (usually the value of an obscured shadow). */
InputValue *Input::secondaryValue()
{
    return &impl->secondaryValue;
}

/*! Sets the primary value. */
void Input::setPrimaryValue(const Value &value)
{
    impl->primaryValue.setValue(value);
}

/*! Sets the secondary value. */
void Input::setSecondaryValue(const Value &value)
{
    impl->secondaryValue.setValue(value);
}

/*! Returns the block which obscures the shadow. */
std::shared_ptr<Block> Input::valueBlock() const
{
    return impl->primaryValue.valueBlock();
}

/*! Returns the ID of the block which obscures the shadow or,
 *  if there's a shadow, returns the block the input points to. */
const std::string &Input::valueBlockId() const
{
    if (valueBlock())
        return valueBlock()->id();
    else
        return impl->primaryValue.valueBlockId();
}

/*!
 * Sets the block which obscures the shadow or,
 * if there's a shadow, sets the block the input points to.
 */
void Input::setValueBlock(std::shared_ptr<Block> block)
{
    impl->primaryValue.setValueBlock(block);
}

/*! Sets the ID of the value block. \see setValueBlock() */
void Input::setValueBlockId(const std::string &id)
{
    impl->primaryValue.setValueBlock(nullptr);
    impl->primaryValue.setValueBlockId(id);
}
