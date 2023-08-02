// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>
#include "engine/compiler.h"

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

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileDefinition(Compiler *compiler);
        static void compileCall(Compiler *compiler);
        static void compileArgument(Compiler *compiler);
};

} // namespace libscratchcpp
