// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Compiler;
class Variable;

/*! \brief The VariableBlocks class contains the implementation of variable blocks. */
class VariableBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            VALUE
        };

        enum Fields
        {
            VARIABLE
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileVariable(Compiler *compiler);
        static void compileSetVariable(Compiler *compiler);
        static void compileChangeVariableBy(Compiler *compiler);
        static void compileShowVariable(Compiler *compiler);
        static void compileHideVariable(Compiler *compiler);

        static void setVarVisible(std::shared_ptr<Variable> var, bool visible, IEngine *engine);

        static unsigned int showGlobalVariable(VirtualMachine *vm);
        static unsigned int showVariable(VirtualMachine *vm);
        static unsigned int hideGlobalVariable(VirtualMachine *vm);
        static unsigned int hideVariable(VirtualMachine *vm);

        static const std::string &variableMonitorName(Block *block);
        static void changeVariableMonitorValue(Block *block, const Value &newValue);
};

} // namespace libscratchcpp
