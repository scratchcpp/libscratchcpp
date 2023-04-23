// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The VariableBlocks class contains the implementation of variable blocks. */
class LIBSCRATCHCPP_EXPORT VariableBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            VALUE
        };

        enum Fields
        {
            VARIABLE
        };

        VariableBlocks();

        std::string name() const override;

        Value setVariable(const BlockArgs &args);
        Value changeVariableBy(const BlockArgs &args);
};

} // namespace libscratchcpp
