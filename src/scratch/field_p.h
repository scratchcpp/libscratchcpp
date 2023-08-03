// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <memory>
#include <scratchcpp/value.h>

namespace libscratchcpp
{

class Entity;

struct FieldPrivate
{
        FieldPrivate(const std::string &name, const Value &value, std::shared_ptr<Entity> valuePtr = nullptr);
        FieldPrivate(const std::string &name, const Value &value, const std::string &valueId);

        std::string name;
        int fieldId = -1;
        Value value;
        std::shared_ptr<Entity> valuePtr = nullptr;
        std::string valueId;
        int specialValueId = -1;
};

} // namespace libscratchcpp
