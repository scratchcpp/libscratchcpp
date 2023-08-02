// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

/*! \brief The LooksBlocks class contains the implementation of looks blocks. */
class LIBSCRATCHCPP_EXPORT LooksBlocks : public IBlockSection
{
    public:
        std::string name() const override;

        void registerBlocks(IEngine *engine) override;
};

} // namespace libscratchcpp
