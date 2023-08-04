// SPDX-License-Identifier: Apache-2.0

#pragma once

// TODO: Use this
//#include "inputvalue.h"
#include <scratchcpp/inputvalue.h>

namespace libscratchcpp
{

/*! \brief The Input class represents a Scratch block input. */
class LIBSCRATCHCPP_EXPORT Input
{
    public:
        enum class Type
        {
            Shadow = 1,        // an editable input, can point to a dropdown menu
            NoShadow = 2,      // points to another block?
            ObscuredShadow = 3 // there's a block inside the shadow block
        };

        Input(std::string name, Type type);
        Input(const Input &) = delete;

        std::string name() const;

        int inputId() const;
        void setInputId(int newInputId);

        Type type() const;

        InputValue *primaryValue();
        InputValue *secondaryValue();

        void setPrimaryValue(Value value);
        void setSecondaryValue(Value value);

        std::shared_ptr<Block> valueBlock() const;
        std::string valueBlockId() const;
        void setValueBlock(std::shared_ptr<Block> block);
        void setValueBlockId(std::string id);

    protected:
        InputValue m_primaryValue;
        InputValue m_secondaryValue;

    private:
        std::string m_name;
        int m_inputId = -1;
        Type m_type;
};

} // namespace libscratchcpp
