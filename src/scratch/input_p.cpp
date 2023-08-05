// SPDX-License-Identifier: Apache-2.0

#include "input_p.h"

using namespace libscratchcpp;

InputPrivate::InputPrivate(const std::string &name, Input::Type type) :
    name(name),
    type(type)
{
}
