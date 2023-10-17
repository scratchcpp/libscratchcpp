// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/keyevent.h>
#include <string>

namespace libscratchcpp
{

struct KeyEventPrivate
{
        KeyEventPrivate(KeyEvent::Type type);
        KeyEventPrivate(const std::string &name);

        void convertNameToLowercase();

        KeyEvent::Type type = KeyEvent::Type::Any;
        std::string name;
};

} // namespace libscratchcpp
