// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

/*! \brief The LooksBlocks class contains the implementation of looks blocks. */
class LooksBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
        };

        enum Fields
        {
        };

        enum FieldValues
        {
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;
};

} // namespace libscratchcpp
