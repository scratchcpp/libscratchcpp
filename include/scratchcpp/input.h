// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class InputValue;
class Value;
class Block;
class InputPrivate;

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

        Input(const std::string &name, Type type);
        Input(const Input &) = delete;

        const std::string &name() const;

        Type type() const;

        InputValue *primaryValue();
        InputValue *secondaryValue();

        void setPrimaryValue(const Value &value);
        void setSecondaryValue(const Value &value);

        Block *valueBlock() const;
        const std::string &valueBlockId() const;
        void setValueBlock(Block *block);
        void setValueBlockId(const std::string &id);

        bool pointsToDropdownMenu() const;
        std::string selectedMenuItem() const;

    private:
        spimpl::unique_impl_ptr<InputPrivate> impl;
};

} // namespace libscratchcpp
