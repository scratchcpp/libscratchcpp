// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class VariableBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileVariable(Compiler *compiler);
        static CompilerValue *compileSetVariableTo(Compiler *compiler);
        static CompilerValue *compileChangeVariableBy(Compiler *compiler);

        static const std::string &variableMonitorName(Block *block);
        static void changeVariableMonitorValue(Block *block, const Value &newValue);
};

} // namespace libscratchcpp
