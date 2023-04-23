// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The ListBlocks class contains the implementation of list blocks. */
class LIBSCRATCHCPP_EXPORT ListBlocks : public IBlockSection
{
    public:
        ListBlocks();

        std::string name() const override;
        bool categoryVisible() const override;
};

} // namespace libscratchcpp
