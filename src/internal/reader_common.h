// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include "../scratch/value.h"
#include <nlohmann/json.hpp>
#include <string>

namespace libscratchcpp
{

Value LIBSCRATCHCPP_EXPORT jsonToValue(nlohmann::json value)
{
    if (value.is_string())
        return value.get<std::string>();
    else if (value.is_number())
        return std::stod(value.dump());
    else if (value.is_boolean())
        return value.get<bool>();
    else
        return value.dump();
}

} // namespace libscratchcpp
