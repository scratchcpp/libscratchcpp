// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class SensingBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileTouchingObject(Compiler *compiler);
        static CompilerValue *compileTouchingColor(Compiler *compiler);
        static CompilerValue *compileColorIsTouchingColor(Compiler *compiler);
};

} // namespace libscratchcpp
