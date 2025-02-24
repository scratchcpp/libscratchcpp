// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/inputvalue.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/block.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <map>
#include <iostream>

#include "inputvalue_p.h"

using namespace libscratchcpp;

static const std::map<ValueType, InputValue::Type>
    VALUE_TYPE_MAP = { { ValueType::Number, InputValue::Type::Number }, { ValueType::Bool, InputValue::Type::String }, { ValueType::String, InputValue::Type::String } };

/*! Constructs InputValue with the given type. */
InputValue::InputValue(Type type) :
    impl(spimpl::make_impl<InputValuePrivate>(type))
{
}

/*! Compiles the input value. */
CompilerValue *InputValue::compile(Compiler *compiler)
{
    switch (impl->type) {
        case Type::Color:
            // TODO: Add support for colors
            return nullptr;

        case Type::Variable: {
            assert(impl->valuePtr);
            Variable *var = dynamic_cast<Variable *>(impl->valuePtr.get());

            if (var)
                return compiler->addVariableValue(var);
            else
                return compiler->addConstValue(Value());
        }

        case Type::List: {
            assert(impl->valuePtr);
            List *list = dynamic_cast<List *>(impl->valuePtr.get());

            if (list)
                return compiler->addListContents(list);
            else
                return compiler->addConstValue(Value());
        }

        default:
            return compiler->addConstValue(impl->value);
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
Block *InputValue::valueBlock() const
{
    return impl->valueBlock;
}

/*! Sets the block. \see Input::setValueBlock() */
void InputValue::setValueBlock(Block *newValueBlock)
{
    impl->valueBlock = newValueBlock;

    if (newValueBlock)
        impl->valueBlockId = newValueBlock->id();
    else
        impl->valueBlockId = "";
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
    impl->valueBlock = nullptr;
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

        if (newValuePtr)
            std::cout << "warning: unsupported input value type (use InputValue::setValueBlock() to set the block)" << std::endl;

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
