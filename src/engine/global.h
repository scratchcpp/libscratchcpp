// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "blockargs.h"
#include <functional>
#include <random>

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
template<typename T>
inline T randint(T start, T end)
{
    if (start > end) {
        auto tmp = start;
        start = end;
        end = tmp;
    }
    std::default_random_engine generator;
    if constexpr (std::is_integral<T>()) {
        std::uniform_int_distribution<T> distribution(start, end);
        return distribution(generator);
    } else {
        std::uniform_real_distribution<T> distribution(start, end);
        return distribution(generator);
    }
}

} // namespace libscratchcpp
