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

} // namespace libscratchcpp
