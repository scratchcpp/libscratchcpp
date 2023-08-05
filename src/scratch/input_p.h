// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>

namespace libscratchcpp
{

struct InputPrivate
{
        InputPrivate(const std::string &name, Input::Type type);
        InputPrivate(const InputValue &) = delete;

        std::string name;
        int inputId = -1;
        Input::Type type;
        InputValue primaryValue;
        InputValue secondaryValue;
};

} // namespace libscratchcpp
