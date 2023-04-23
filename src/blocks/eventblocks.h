// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The EventBlocks class contains the implementation of event blocks. */
class LIBSCRATCHCPP_EXPORT EventBlocks : public IBlockSection
{
    public:
        EventBlocks();

        std::string name() const override;
};

} // namespace libscratchcpp
