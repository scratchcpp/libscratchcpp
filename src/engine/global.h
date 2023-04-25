// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "blockargs.h"
#include <functional>
#include <vector>

/*! \brief The main namespace of the library. */
namespace libscratchcpp
{

/*!
 * \typedef BlockImpl
 *
 * BlockImpl is used to store block implementation functions as std::function in blocks.
 */
using BlockImpl = std::function<Value(const BlockArgs &)>;

/*! Generates a random number in the given interval like the random.randint() function in Python. */
inline int randint(int start, int end)
{
    if (start > end) {
        int tmp = start;
        start = end;
        end = tmp;
    }
    return rand() % (end - start + 1) + start;
}

} // namespace libscratchcpp
