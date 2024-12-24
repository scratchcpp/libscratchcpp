// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class IRandomGenerator;

class OperatorBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileAdd(Compiler *compiler);
        static CompilerValue *compileSubtract(Compiler *compiler);
        static CompilerValue *compileMultiply(Compiler *compiler);
        static CompilerValue *compileDivide(Compiler *compiler);
        static CompilerValue *compileRandom(Compiler *compiler);
        static CompilerValue *compileLt(Compiler *compiler);
        static CompilerValue *compileEquals(Compiler *compiler);
        static CompilerValue *compileGt(Compiler *compiler);
        static CompilerValue *compileAnd(Compiler *compiler);
        static CompilerValue *compileOr(Compiler *compiler);
        static CompilerValue *compileNot(Compiler *compiler);
        static CompilerValue *compileJoin(Compiler *compiler);
        static CompilerValue *compileLetterOf(Compiler *compiler);
        static CompilerValue *compileLength(Compiler *compiler);
        static CompilerValue *compileContains(Compiler *compiler);
        static CompilerValue *compileMod(Compiler *compiler);
};

} // namespace libscratchcpp
