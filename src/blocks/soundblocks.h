// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The SoundBlocks class contains the implementation of sound blocks. */
class LIBSCRATCHCPP_EXPORT SoundBlocks : public IBlockSection
{
    public:
	SoundBlocks();

        std::string name() const override;
};

} // namespace libscratchcpp
