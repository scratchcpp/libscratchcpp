// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The VariableBlocks class contains the implementation of variable and list blocks. */
class LIBSCRATCHCPP_EXPORT VariableBlocks : public IBlockSection
{
    public:
	VariableBlocks();

        std::string name() const override;
};

} // namespace libscratchcpp
