// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

/*! \brief The SensingBlocks class contains the implementation of sensing blocks. */
class SensingBlocks : public IBlockSection
{
    public:
        std::string name() const override;

        void registerBlocks(IEngine *engine) override;
};

} // namespace libscratchcpp
