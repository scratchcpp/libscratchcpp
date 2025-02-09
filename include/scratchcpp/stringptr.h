// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "global.h"

namespace libscratchcpp
{

extern "C"
{
    /*! \brief The StringPtr struct holds a string data pointer and string size */
    struct LIBSCRATCHCPP_EXPORT StringPtr
    {
            // NOTE: Constructors and destructors only work in C++ code and are not supposed to be used in LLVM IR
            StringPtr() = default;
            StringPtr(const StringPtr &) = delete;

            ~StringPtr()
            {
                if (data && allocatedSize > 0)
                    free(data);
            }

            // NOTE: Any changes must also be done in the LLVM code builder!
            char16_t *data = nullptr;
            size_t size = 0;
            size_t allocatedSize = 0;
    };
}

} // namespace libscratchcpp
