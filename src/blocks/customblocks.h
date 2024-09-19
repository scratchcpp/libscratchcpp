// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class Compiler;

class CustomBlocks : public IExtension
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
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileDefinition(Compiler *compiler);
        static void compileCall(Compiler *compiler);
        static void compileArgument(Compiler *compiler);
};

} // namespace libscratchcpp
