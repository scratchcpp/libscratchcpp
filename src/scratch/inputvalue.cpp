// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/inputvalue.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/entity.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <map>

#include "inputvalue_p.h"

using namespace libscratchcpp;

static const std::map<Value::Type, InputValue::Type> VALUE_TYPE_MAP = {
    { Value::Type::Number, InputValue::Type::Number },           { Value::Type::Bool, InputValue::Type::String },
    { Value::Type::String, InputValue::Type::String },           { Value::Type::Infinity, InputValue::Type::String },
    { Value::Type::NegativeInfinity, InputValue::Type::String }, { Value::Type::NaN, InputValue::Type::String }
};

/*! Constructs InputValue with the given type. */
InputValue::InputValue(Type type) :
    impl(spimpl::make_impl<InputValuePrivate>(type))
{
}

/*! Compiles the input value. */
void InputValue::compile(Compiler *compiler)
{
    switch (impl->type) {
        case Type::Color:
            // TODO: Add support for colors
            break;

        case Type::Broadcast:
            // TODO: Add support for broadcasts
            break;

        case Type::Variable:
            compiler->addInstruction(vm::OP_READ_VAR, { compiler->variableIndex(impl->valuePtr) });
            break;

        case Type::List:
            compiler->addInstruction(vm::OP_READ_LIST, { compiler->listIndex(impl->valuePtr) });
            break;

        default:
            compiler->addInstruction(vm::OP_CONST, { compiler->constIndex(this) });
            break;
    }
}

/*! Returns the type of the value. */
InputValue::Type InputValue::type() const
{
    return impl->type;
}

/*! Sets the type of the value. */
void InputValue::setType(Type newType)
{
    impl->type = newType;
}

/*! Returns the value. */
const Value &InputValue::value() const
{
    return impl->value;
}

/*! Sets the value. */
void InputValue::setValue(const Value &newValue)
{
    setType(VALUE_TYPE_MAP.at(impl->value.type()));
    impl->value = newValue;
}

/*! Returns the block. \see Input::valueBlock() */
const std::shared_ptr<Block> &InputValue::valueBlock() const
{
    return impl->valueBlock;
}

/*! Sets the block. \see Input::setValueBlock() */
void InputValue::setValueBlock(const std::shared_ptr<Block> &newValueBlock)
{
    impl->valueBlock = newValueBlock;
}

/*! Returns the ID of the block. \see valueBlock() */
const std::string &InputValue::valueBlockId() const
{
    return impl->valueBlockId;
}

/*! Sets the ID of the block. \see setValueBlock() */
void InputValue::setValueBlockId(const std::string &newValueBlockId)
{
    impl->valueBlockId = newValueBlockId;
}

/*! Returns a pointer to the value (e. g. a variable). */
std::shared_ptr<Entity> InputValue::valuePtr() const
{
    return impl->valuePtr;
}

/*! Sets the value pointer. */
void InputValue::setValuePtr(const std::shared_ptr<Entity> &newValuePtr)
{
    if (std::dynamic_pointer_cast<Broadcast>(newValuePtr))
        setType(Type::Broadcast);
    else if (std::dynamic_pointer_cast<Variable>(newValuePtr))
        setType(Type::Variable);
    else if (std::dynamic_pointer_cast<List>(newValuePtr))
        setType(Type::List);
    else {
        impl->valuePtr = nullptr;
        impl->valueId = "";
        return;
    }

    impl->valuePtr = newValuePtr;
    impl->valueId = newValuePtr->id();
}

/*! Returns the ID of the value. */
const std::string &InputValue::valueId() const
{
    if (impl->valuePtr)
        return impl->valuePtr->id();
    else
        return impl->valueId;
}

void InputValue::setValueId(const std::string &newValueId)
{
    impl->valuePtr = nullptr;
    impl->valueId = newValueId;
}
