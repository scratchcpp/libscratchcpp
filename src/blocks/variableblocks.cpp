// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/field.h>

#include "variableblocks.h"

using namespace libscratchcpp;

std::string VariableBlocks::name() const
{
    return "Variables";
}

void VariableBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "data_setvariableto", &compileSetVariable);
    engine->addCompileFunction(this, "data_changevariableby", &compileChangeVariableBy);

    // Inputs
    engine->addInput(this, "VALUE", VALUE);

    // Fields
    engine->addField(this, "VARIABLE", VARIABLE);
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
