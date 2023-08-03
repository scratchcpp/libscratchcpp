// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <scratchcpp/variable.h>

#include "input.h"
#include "block.h"

using namespace libscratchcpp;

/*! Constructs Input. */
Input::Input(std::string name, Type type) :
    m_name(name),
    m_type(type)
{
}

/*! Returns the name of the input. */
std::string Input::name() const
{
    return m_name;
}

/*! Returns the ID of the input. */
int Input::inputId() const
{
    return m_inputId;
}

/*! Sets the ID of the input. */
void Input::setInputId(int newInputId)
{
    m_inputId = newInputId;
}

/*! Returns the type of the input. */
Input::Type Input::type() const
{
    return m_type;
}

/*! Returns the primary value. */
InputValue *Input::primaryValue()
{
    return &m_primaryValue;
}

/*! Returns the secondary value (usually the value of an obscured shadow). */
InputValue *Input::secondaryValue()
{
    return &m_secondaryValue;
}

/*! Sets the primary value. */
void Input::setPrimaryValue(Value value)
{
    m_primaryValue.setValue(value);
}

/*! Sets the secondary value. */
void Input::setSecondaryValue(Value value)
{
}

/*! Returns the block which obscures the shadow. */
std::shared_ptr<Block> Input::valueBlock() const
{
    return m_primaryValue.valueBlock();
}

/*! Returns the ID of the block which obscures the shadow or,
 *  if there's a shadow, returns the block the input points to. */
std::string Input::valueBlockId() const
{
    if (valueBlock())
        return valueBlock()->id();
    else
        return m_primaryValue.valueBlockId();
}

/*!
 * Sets the block which obscures the shadow or,
 * if there's a shadow, sets the block the input points to.
 */
void Input::setValueBlock(std::shared_ptr<Block> block)
{
    m_primaryValue.setValueBlock(block);
}

/*! Sets the ID of the value block. \see setValueBlock() */
void Input::setValueBlockId(std::string id)
{
    m_primaryValue.setValueBlockId(id);
    m_primaryValue.setValueBlock(nullptr);
}
