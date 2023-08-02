// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

/*! \brief The SoundBlocks class contains the implementation of sound blocks. */
class LIBSCRATCHCPP_EXPORT SoundBlocks : public IBlockSection
{
    public:
        std::string name() const override;

        void registerBlocks(IEngine *engine) override;
};

} // namespace libscratchcpp
