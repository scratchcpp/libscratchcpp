// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The OperatorBlocks class contains the implementation of operator blocks. */
class LIBSCRATCHCPP_EXPORT OperatorBlocks : public IBlockSection
{
    public:
        OperatorBlocks();

        std::string name() const override;
};

} // namespace libscratchcpp
