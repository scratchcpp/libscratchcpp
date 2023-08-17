// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/entity.h>

#include "field_p.h"

using namespace libscratchcpp;

FieldPrivate::FieldPrivate(const std::string &name, const Value &value, std::shared_ptr<Entity> valuePtr) :
    name(name),
    value(value),
    valuePtr(valuePtr)
{
    if (valuePtr)
        valueId = valuePtr->id();
}

FieldPrivate::FieldPrivate(const std::string &name, const Value &value, const std::string &valueId) :
    name(name),
    value(value),
    valueId(valueId)
{
}
