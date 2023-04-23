// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "value.h"
#include <memory>

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT Block;

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

        Type type() const;
        void setType(Type newType);

        const Value &value() const;
        void setValue(const Value &newValue);

        const std::shared_ptr<Block> &valueBlock() const;
        void setValueBlock(const std::shared_ptr<Block> &newValueBlock);

        const std::string &valueBlockId() const;
        void setValueBlockId(const std::string &newValueBlockId);

    private:
        Type m_type;
        Value m_value;
        std::shared_ptr<Block> m_valueBlock = nullptr;
        std::string m_valueBlockId;
};

} // namespace libscratchcpp
