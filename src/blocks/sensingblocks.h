// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The SensingBlocks class contains the implementation of sensing blocks. */
class LIBSCRATCHCPP_EXPORT SensingBlocks : public IBlockSection
{
    public:
        SensingBlocks();

        std::string name() const override;
};

} // namespace libscratchcpp
