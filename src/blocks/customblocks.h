// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

class CustomBlocks : public IBlockSection
{
    public:
        CustomBlocks();

        std::string name() const override;

        static void compileCall(Compiler *compiler);
};

} // namespace libscratchcpp
