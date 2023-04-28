// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../input.h"

namespace libscratchcpp
{

class ListInput : public Input
{
    public:
        ListInput(std::string name, Type type);

        Value value() const override;
};

} // namespace libscratchcpp
