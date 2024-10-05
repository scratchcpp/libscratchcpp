// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "spimpl.h"
#include "value.h"

namespace libscratchcpp
{

class Block;
class Entity;
class InputValuePrivate;

/*! \brief The InputValue class provides methods for the value of an Input. */
class LIBSCRATCHCPP_EXPORT InputValue
{
    public:
        enum class Type
        {
            Number = 4,
            PositiveNumber = 5,
            PositiveInteger = 6,
            Integer = 7,
            Angle = 8,
            Color = 9,
            String = 10,
            Broadcast = 11,
            Variable = 12,
            List = 13
        };

        InputValue(Type type = Type::Number);

        void compile(Compiler *compiler);

        Type type() const;
        void setType(Type newType);

        const Value &value() const;
        void setValue(const Value &newValue);

        const std::shared_ptr<Block> &valueBlock() const;
        void setValueBlock(const std::shared_ptr<Block> &newValueBlock);

        const std::string &valueBlockId() const;
        void setValueBlockId(const std::string &newValueBlockId);

        std::shared_ptr<Entity> valuePtr() const;
        void setValuePtr(const std::shared_ptr<Entity> &newValuePtr);

        const std::string &valueId() const;
        void setValueId(const std::string &newValueId);

    private:
        spimpl::impl_ptr<InputValuePrivate> impl;
};

} // namespace libscratchcpp
