// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value.h>

namespace libscratchcpp
{

struct VariablePrivate
{
        VariablePrivate(const std::string &name, const Value &value = Value(), bool isCloudVariable = false);
        VariablePrivate(const VariablePrivate &) = delete;

        std::string name;
        Value value;
        bool isCloudVariable;
};

} // namespace libscratchcpp
