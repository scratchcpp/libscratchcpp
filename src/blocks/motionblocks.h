// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The MotionBlocks class contains the implementation of motion blocks. */
class LIBSCRATCHCPP_EXPORT MotionBlocks : public IBlockSection
{
    public:
        MotionBlocks();

        std::string name() const override;
};

} // namespace libscratchcpp
