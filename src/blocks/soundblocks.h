// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class IAudioOutput;

class SoundBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;
        void onInit(IEngine *engine) override;

    private:
        static CompilerValue *compilePlay(Compiler *compiler);
        static CompilerValue *compilePlayUntilDone(Compiler *compiler);
        static CompilerValue *compileStopAllSounds(Compiler *compiler);
        static CompilerValue *compileSetEffectTo(Compiler *compiler);
        static CompilerValue *compileChangeEffectBy(Compiler *compiler);
        static CompilerValue *compileClearEffects(Compiler *compiler);
        static CompilerValue *compileChangeVolumeBy(Compiler *compiler);
        static CompilerValue *compileSetVolumeTo(Compiler *compiler);
};

} // namespace libscratchcpp
