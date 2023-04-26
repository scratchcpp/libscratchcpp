// SPDX-License-Identifier: Apache-2.0

#include "input.h"
#include "list.h"
#include "block.h"
#include "variable.h"
#include <iostream>

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

/*!
 * Returns the value of the input.\n
 * If there's an obscured shadow, the value will
 * be retrieved from the block that obscures the shadow.\n
 * Use secondaryValue() to get the value of the shadow.
 */
Value Input::value() const
{
    switch (m_type) {
        case Type::Shadow:
            return m_primaryValue.value();
        case Type::NoShadow:
        case Type::ObscuredShadow: {
            switch (m_primaryValue.type()) {
                case InputValue::Type::Variable: {
                    auto variable = std::static_pointer_cast<Variable>(m_primaryValue.valuePtr());
                    if (!variable) {
                        std::cout << "warning: attempted to read a null variable" << std::endl;
                        return Value();
                    }
                    return variable->value();
                }
                case InputValue::Type::List: {
                    auto list = std::static_pointer_cast<List>(m_primaryValue.valuePtr());
                    if (!list) {
                        std::cout << "warning: attempted to read a null list" << std::endl;
                        return Value();
                    }
                    return list->toString();
                }
                default: {
                    auto block = m_primaryValue.valueBlock();
                    if (!block) {
                        if (m_type == Type::ObscuredShadow)
                            std::cout << "warning: attempted to get value of an input shadow obscured by a null block" << std::endl;
                        return Value();
                    }
                    return block->run();
                }
            }
        }
    }
    return Value();
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
