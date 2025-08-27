// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "global.h"

namespace libscratchcpp
{

struct StringPtr;

enum class LIBSCRATCHCPP_EXPORT ValueType
{
    Number = 0,
    Bool = 1,
    String = 2,
    Pointer = 3
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
                    StringPtr *stringValue;
                    const void *pointerValue;
            };

            ValueType type;
    };
}

} // namespace libscratchcpp
