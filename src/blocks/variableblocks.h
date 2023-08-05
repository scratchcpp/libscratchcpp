// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Compiler;

/*! \brief The VariableBlocks class contains the implementation of variable blocks. */
class VariableBlocks : public IBlockSection
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

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileSetVariable(Compiler *compiler);
        static void compileChangeVariableBy(Compiler *compiler);
};

} // namespace libscratchcpp
