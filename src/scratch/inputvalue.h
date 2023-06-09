// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "value.h"
#include "ientity.h"
#include <memory>

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT Block;
class LIBSCRATCHCPP_EXPORT Compiler;

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

        InputValue();
        InputValue(Type type);

        void compile(Compiler *compiler);

        Type type() const;
        void setType(Type newType);

        const Value &value() const;
        void setValue(const Value &newValue);

        const std::shared_ptr<Block> &valueBlock() const;
        void setValueBlock(const std::shared_ptr<Block> &newValueBlock);

        const std::string &valueBlockId() const;
        void setValueBlockId(const std::string &newValueBlockId);

        std::shared_ptr<IEntity> valuePtr() const;
        void setValuePtr(const std::shared_ptr<IEntity> &newValuePtr);

        std::string valueId() const;
        void setValueId(const std::string &newValueId);

    private:
        Type m_type;
        Value m_value;
        std::shared_ptr<Block> m_valueBlock = nullptr;
        std::string m_valueBlockId;
        std::shared_ptr<IEntity> m_valuePtr = nullptr;
        std::string m_valueId;
};

} // namespace libscratchcpp
