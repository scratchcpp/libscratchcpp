// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class MotionBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileMoveSteps(Compiler *compiler);
        static CompilerValue *compileTurnRight(Compiler *compiler);
        static CompilerValue *compileTurnLeft(Compiler *compiler);
        static CompilerValue *compilePointInDirection(Compiler *compiler);
        static CompilerValue *compilePointTowards(Compiler *compiler);
        static CompilerValue *compileGoToXY(Compiler *compiler);
        static CompilerValue *compileGoTo(Compiler *compiler);
        static CompilerValue *compileGlideSecsToXY(Compiler *compiler);
        static CompilerValue *compileGlideTo(Compiler *compiler);
        static CompilerValue *compileChangeXBy(Compiler *compiler);
        static CompilerValue *compileSetX(Compiler *compiler);
        static CompilerValue *compileChangeYBy(Compiler *compiler);
};

} // namespace libscratchcpp
