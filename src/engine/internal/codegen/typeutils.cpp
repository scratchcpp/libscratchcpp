// SPDX-License-Identifier: Apache-2.0

#include <unordered_map>

#include "typeutils.h"

using namespace libscratchcpp;

static std::unordered_map<ValueType, Compiler::StaticType> TYPE_MAP = {
    { ValueType::Number, Compiler::StaticType::Number },
    { ValueType::Bool, Compiler::StaticType::Bool },
    { ValueType::String, Compiler::StaticType::String },
    { ValueType::Pointer, Compiler::StaticType::Pointer }
};

static std::unordered_map<Compiler::StaticType, ValueType> REVERSE_TYPE_MAP = {
    { Compiler::StaticType::Number, ValueType::Number },
    { Compiler::StaticType::Bool, ValueType::Bool },
    { Compiler::StaticType::String, ValueType::String },
    { Compiler::StaticType::Pointer, ValueType::Pointer }
};

Compiler::StaticType TypeUtils::mapType(ValueType type)
{
    assert(TYPE_MAP.find(type) != TYPE_MAP.cend());
    return TYPE_MAP[type];
}

ValueType TypeUtils::mapType(Compiler::StaticType type)
{
    assert(REVERSE_TYPE_MAP.find(type) != REVERSE_TYPE_MAP.cend());
    return REVERSE_TYPE_MAP[type];
}

bool TypeUtils::isSingleType(Compiler::StaticType type)
{
    // Check if the type is a power of 2 (only one bit set)
    return (type & (static_cast<Compiler::StaticType>(static_cast<int>(type) - 1))) == static_cast<Compiler::StaticType>(0);
}
