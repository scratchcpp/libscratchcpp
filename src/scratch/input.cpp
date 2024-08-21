// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
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

/*!
 * Returns true if the input points to a dropdown menu.\n
 * (if valueBlock() points to a shadow block with a single field which does not point to an entity)
 */
bool Input::pointsToDropdownMenu() const
{
    auto block = valueBlock();

    if (!block || !block->shadow())
        return false;

    const auto &fields = block->fields();

    if (fields.size() != 1)
        return false;

    auto field = fields[0];
    return (field && !field->valuePtr() && (field->valueId() == ""));
}

/*!
 * Returns the selected item in the dropdown menu.\n
 * Works only pointsToDropdownMenu() is true.
 */
std::string Input::selectedMenuItem() const
{
    if (!pointsToDropdownMenu())
        return "";

    return valueBlock()->fieldAt(0)->value().toString();
}
