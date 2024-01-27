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
    else if (value.is_number()) {
        // Set locale to C to avoid conversion issues
        std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
        std::setlocale(LC_NUMERIC, "C");

        double converted = std::stod(value.dump());

        // Restore old locale
        std::setlocale(LC_NUMERIC, oldLocale.c_str());

        return converted;
    } else if (value.is_boolean())
        return value.get<bool>();
    else
        return value.dump();
}

} // namespace libscratchcpp
