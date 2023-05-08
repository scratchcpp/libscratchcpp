// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../libscratchcpp_global.h"
#include <functional>
#include <random>

/*! \brief The main namespace of the library. */
namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT VirtualMachine;
class LIBSCRATCHCPP_EXPORT Compiler;

/*!
 * \typedef BlockFunc
 *
 * BlockFunc is a function pointer for block implementation functions.
 */
using BlockFunc = unsigned int (*)(VirtualMachine *vm);

/*!
 * \typedef BlockComp
 *
 * BlockComp is a function pointer for functions which are used to compile blocks to bytecode.
 */
using BlockComp = void (*)(Compiler *);

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
