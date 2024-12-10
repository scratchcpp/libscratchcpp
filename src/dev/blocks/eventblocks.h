// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class EventBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileWhenTouchingObject(Compiler *compiler);
        static CompilerValue *compileWhenFlagClicked(Compiler *compiler);
        static CompilerValue *compileWhenThisSpriteClicked(Compiler *compiler);
};

} // namespace libscratchcpp
