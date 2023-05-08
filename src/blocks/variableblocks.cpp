// SPDX-License-Identifier: Apache-2.0

#include "variableblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

VariableBlocks::VariableBlocks()
{
    // Blocks
    addCompileFunction("data_setvariableto", &compileSetVariable);
    addCompileFunction("data_changevariableby", &compileChangeVariableBy);

    // Inputs
    addInput("VALUE", VALUE);

    // Fields
    addField("VARIABLE", VARIABLE);
}

std::string VariableBlocks::name() const
{
    return "Variables";
}

void VariableBlocks::compileSetVariable(Compiler *compiler)
{
    compiler->addInput(VALUE);
    compiler->addInstruction(vm::OP_SET_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
}

void VariableBlocks::compileChangeVariableBy(Compiler *compiler)
{
    compiler->addInput(VALUE);
    compiler->addInstruction(vm::OP_CHANGE_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
}
