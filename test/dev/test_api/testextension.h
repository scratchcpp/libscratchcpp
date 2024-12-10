#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class TestExtension : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileSimple(Compiler *compiler);
        static CompilerValue *compilePrint(Compiler *compiler);
        static CompilerValue *compilePrintDropdown(Compiler *compiler);
        static CompilerValue *compilePrintField(Compiler *compiler);
        static CompilerValue *compileTestStr(Compiler *compiler);
        static CompilerValue *compileInput(Compiler *compiler);
};

} // namespace libscratchcpp