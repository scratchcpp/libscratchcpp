// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/value_functions.h>

namespace libscratchcpp
{

// Owns ValueData returned by a native call. Boxed/borrowed values use ValueData.
class CppOwnedValue final : public ValueData
{
    public:
        CppOwnedValue() : ValueData{} { }
        ~CppOwnedValue() { value_free(this); }

        CppOwnedValue(const CppOwnedValue &) = delete;
        CppOwnedValue &operator=(const CppOwnedValue &) = delete;

        // Adopt a newly returned value, releasing any previous loop iteration's result.
        CppOwnedValue &operator=(ValueData value)
        {
            value_free(this);
            static_cast<ValueData &>(*this) = value;
            return *this;
        }
};

} // namespace libscratchcpp
