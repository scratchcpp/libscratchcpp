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
        ~LooksBlocks();

        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;
        void onInit(IEngine *engine) override;

        static inline IGraphicsEffect *getEffect(IEngine *engine, long index) { return m_instances[engine]->m_effects[index]; }

    private:
        static void compileSayOrThinkForSecs(Compiler *compiler, const std::string function);
        static long getEffectIndex(IEngine *engine, const std::string &name);

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

        IEngine *m_engine = nullptr;
        std::unordered_map<std::string, long> m_effectMap;
        std::vector<IGraphicsEffect *> m_effects;
        static inline std::unordered_map<IEngine *, LooksBlocks *> m_instances;
};

} // namespace libscratchcpp
