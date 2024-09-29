// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/inputvalue.h>
#ifndef USE_LLVM
#include <scratchcpp/compiler.h>
#endif
#include <scratchcpp/block.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <map>
#include <iostream>

#include "inputvalue_p.h"

using namespace libscratchcpp;

static const std::map<ValueType, InputValue::Type> VALUE_TYPE_MAP = {
    { ValueType::Integer, InputValue::Type::Number }, { ValueType::Double, InputValue::Type::Number },   { ValueType::Bool, InputValue::Type::String },
    { ValueType::String, InputValue::Type::String },  { ValueType::Infinity, InputValue::Type::String }, { ValueType::NegativeInfinity, InputValue::Type::String },
    { ValueType::NaN, InputValue::Type::String }
};

/*! Constructs InputValue with the given type. */
InputValue::InputValue(Type type) :
    impl(spimpl::make_impl<InputValuePrivate>(type))
{
}

#ifndef USE_LLVM
/*! Compiles the input value. */
void InputValue::compile(Compiler *compiler)
{
    switch (impl->type) {
        case Type::Color:
            // TODO: Add support for colors
            break;

        case Type::Variable:
            assert(impl->valuePtr);
            compiler->addInstruction(vm::OP_READ_VAR, { compiler->variableIndex(impl->valuePtr) });
            break;

        case Type::List:
            assert(impl->valuePtr);
            compiler->addInstruction(vm::OP_READ_LIST, { compiler->listIndex(impl->valuePtr) });
            break;

        default:
            compiler->addInstruction(vm::OP_CONST, { compiler->constIndex(this) });
            break;
    }
}
#endif // USE_LLVM

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
