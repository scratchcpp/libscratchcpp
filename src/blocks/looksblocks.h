// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>
#include <scratchcpp/textbubble.h>
#include <unordered_map>

namespace libscratchcpp
{

class Target;
class Thread;
class IGraphicsEffect;

class LooksBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;
        void onInit(IEngine *engine) override;

    private:
        static void compileSayOrThinkForSecs(Compiler *compiler, const std::string &function);
        static void compileSetOrChangeEffect(Compiler *compiler, const std::string &function, const std::string &effectName, CompilerValue *arg);

        static CompilerValue *compileSayForSecs(Compiler *compiler);
        static CompilerValue *compileSay(Compiler *compiler);
        static CompilerValue *compileThinkForSecs(Compiler *compiler);
        static CompilerValue *compileThink(Compiler *compiler);
        static CompilerValue *compileShow(Compiler *compiler);
        static CompilerValue *compileHide(Compiler *compiler);
        static CompilerValue *compileChangeEffectBy(Compiler *compiler);
        static CompilerValue *compileSetEffectTo(Compiler *compiler);
        static CompilerValue *compileClearGraphicEffects(Compiler *compiler);
        static CompilerValue *compileChangeSizeBy(Compiler *compiler);
        static CompilerValue *compileSetSizeTo(Compiler *compiler);
        static CompilerValue *compileSize(Compiler *compiler);
        static CompilerValue *compileSwitchCostumeTo(Compiler *compiler);
};

} // namespace libscratchcpp
