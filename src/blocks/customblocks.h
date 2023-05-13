// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

class CustomBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            CUSTOM_BLOCK
        };

        enum Fields
        {
            VALUE
        };

        CustomBlocks();

        std::string name() const override;

        static void compileDefinition(Compiler *compiler);
        static void compileCall(Compiler *compiler);
        static void compileArgument(Compiler *compiler);
};

} // namespace libscratchcpp
