// SPDX-License-Identifier: Apache-2.0

#include "inputvalue.h"
#include <map>

using namespace libscratchcpp;

static const std::map<Value::Type, InputValue::Type> VALUE_TYPE_MAP = {
    { Value::Type::Number, InputValue::Type::Number },
    { Value::Type::Bool, InputValue::Type::String },
    { Value::Type::String, InputValue::Type::String },
    { Value::Type::Special, InputValue::Type::String }
};

/*! Constructs InputValue. */
InputValue::InputValue()
{
}

/*! Constructs InputValue with the given type. */
InputValue::InputValue(Type type) :
    m_type(type)
{
}

/*! Returns the type of the value. */
InputValue::Type InputValue::type() const
{
    return m_type;
}

/*! Sets the type of the value. */
void InputValue::setType(Type newType)
{
    m_type = newType;
}

/*! Returns the value. */
const Value &InputValue::value() const
{
    return m_value;
}

/*! Sets the value. */
void InputValue::setValue(const Value &newValue)
{
    setType(VALUE_TYPE_MAP.at(m_value.type()));
    m_value = newValue;
}

/*! Returns the block. \see Input::valueBlock() */
const std::shared_ptr<Block> &InputValue::valueBlock() const
{
    return m_valueBlock;
}

/*! Sets the block. \see Input::setValueBlock() */
void InputValue::setValueBlock(const std::shared_ptr<Block> &newValueBlock)
{
    m_valueBlock = newValueBlock;
}

/*! Returns the ID of the block. \see valueBlock() */
const std::string &InputValue::valueBlockId() const
{
    return m_valueBlockId;
}

/*! Sets the ID of the block. \see setValueBlock() */
void InputValue::setValueBlockId(const std::string &newValueBlockId)
{
    m_valueBlockId = newValueBlockId;
}

/*! Returns a pointer to the value (e. g. a variable). */
std::shared_ptr<IEntity> InputValue::valuePtr() const
{
    return m_valuePtr;
}

/*! Sets the value pointer. */
void InputValue::setValuePtr(const std::shared_ptr<IEntity> &newValuePtr)
{
    m_valuePtr = newValuePtr;
}

/*! Returns the ID of the value. */
std::string InputValue::valueId() const
{
    if (m_valuePtr)
        return m_valuePtr->id();
    else
        return m_valueId;
}

void InputValue::setValueId(const std::string &newValueId)
{
    m_valuePtr = nullptr;
    m_valueId = newValueId;
}
