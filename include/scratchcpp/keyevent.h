// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class KeyEventPrivate;

/*! \brief The KeyEvent class represents a Scratch key event. */
class LIBSCRATCHCPP_EXPORT KeyEvent
{
    public:
        enum class Type
        {
            Any,
            Space,
            Left,
            Up,
            Right,
            Down,
            Enter
        };

        KeyEvent(Type type = Type::Any);
        KeyEvent(const std::string &name);

        Type type() const;
        const std::string &name() const;

        friend bool operator==(const KeyEvent &ev1, const KeyEvent &ev2) { return ev1.name() == ev2.name(); }

    private:
        spimpl::impl_ptr<KeyEventPrivate> impl;
};

} // namespace libscratchcpp
