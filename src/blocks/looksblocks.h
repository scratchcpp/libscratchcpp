// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The LooksBlocks class contains the implementation of looks blocks. */
class LIBSCRATCHCPP_EXPORT LooksBlocks : public IBlockSection
{
    public:
        LooksBlocks();

        std::string name() const override;
};

} // namespace libscratchcpp
