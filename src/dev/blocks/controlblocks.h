// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class ControlBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileForever(Compiler *compiler);
        static CompilerValue *compileRepeat(Compiler *compiler);
};

} // namespace libscratchcpp
