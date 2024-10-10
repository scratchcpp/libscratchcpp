// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "global.h"

namespace libscratchcpp
{

enum class LIBSCRATCHCPP_EXPORT SpecialValue
{
    Infinity,
    NegativeInfinity,
    NaN
};

enum class LIBSCRATCHCPP_EXPORT ValueType
{
    Number = 0,
    Bool = 1,
    String = 2,
    Infinity = -1,
    NegativeInfinity = -2,
    NaN = -3
};

extern "C"
{
    /*! \brief The ValueData struct holds the data of Value. It's used in compiled Scratch code for better performance. */
    struct LIBSCRATCHCPP_EXPORT ValueData
    {
            // NOTE: Any changes must also be done in the LLVM code builder!
            union
            {
                    double numberValue;
                    bool boolValue;
                    char *stringValue;
            };

            ValueType type;
            size_t stringSize; // allocated size, not length
    };
}

} // namespace libscratchcpp
