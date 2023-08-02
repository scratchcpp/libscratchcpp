// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>
#include "engine/compiler.h"

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

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileSetVariable(Compiler *compiler);
        static void compileChangeVariableBy(Compiler *compiler);
};

} // namespace libscratchcpp
