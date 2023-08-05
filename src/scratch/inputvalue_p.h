// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/inputvalue.h>

namespace libscratchcpp
{

struct InputValuePrivate
{
        InputValuePrivate();
        InputValuePrivate(InputValue::Type type);

        InputValue::Type type;
        Value value;
        std::shared_ptr<Block> valueBlock = nullptr;
        std::string valueBlockId;
        std::shared_ptr<Entity> valuePtr = nullptr;
        std::string valueId;
};

} // namespace libscratchcpp
