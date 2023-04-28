// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../input.h"

namespace libscratchcpp
{

class BlockInput : public Input
{
    public:
        BlockInput(std::string name, Type type);

        Value value() const override;
};

} // namespace libscratchcpp
