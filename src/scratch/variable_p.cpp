// SPDX-License-Identifier: Apache-2.0

#include "variable_p.h"

using namespace libscratchcpp;

VariablePrivate::VariablePrivate(const std::string &name, const Value &value, bool isCloudVariable) :
    name(name),
    value(value),
    isCloudVariable(isCloudVariable)
{
}
