// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>
#include <scratchcpp/textbubble.h>
#include <unordered_map>

namespace libscratchcpp
{

class Target;
class Thread;

class LooksBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;
        void onInit(IEngine *engine) override;

    private:
        static void compileSayOrThinkForSecs(Compiler *compiler, const std::string function);
        static CompilerValue *compileSayForSecs(Compiler *compiler);
        static CompilerValue *compileSay(Compiler *compiler);
        static CompilerValue *compileThinkForSecs(Compiler *compiler);
        static CompilerValue *compileThink(Compiler *compiler);
};

} // namespace libscratchcpp
