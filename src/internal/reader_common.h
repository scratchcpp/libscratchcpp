// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <scratchcpp/value.h>

namespace libscratchcpp
{

Value jsonToValue(nlohmann::json value)
{
    if (value.is_string())
        return value.get<std::string>();
    else if (value.is_boolean())
        return value.get<bool>();
    else if (value.is_number_integer() || value.is_number_unsigned())
        return value.get<long>();
    else if (value.is_number_float())
        return value.get<double>();
    else {
        assert(!value.is_number());
        return value.dump();
    }
}

} // namespace libscratchcpp
