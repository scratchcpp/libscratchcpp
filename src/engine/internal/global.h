// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include <random>

namespace libscratchcpp
{

/*! Generates a random number in the given interval like the random.randint() function in Python. */
template<typename T>
inline T randint(T start, T end)
{
    if (start > end) {
        auto tmp = start;
        start = end;
        end = tmp;
    }
    std::random_device d;
    std::default_random_engine generator(d());
    if constexpr (std::is_integral<T>()) {
        std::uniform_int_distribution<T> distribution(start, end);
        return distribution(generator);
    } else {
        std::uniform_real_distribution<T> distribution(start, end);
        return distribution(generator);
    }
}

} // namespace libscratchcpp
