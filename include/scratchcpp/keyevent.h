// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class KeyEventPrivate;

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

    private:
        spimpl::impl_ptr<KeyEventPrivate> impl;
};

} // namespace libscratchcpp
