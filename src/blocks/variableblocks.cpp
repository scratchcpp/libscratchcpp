// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>

#include "variableblocks.h"

using namespace libscratchcpp;

std::string VariableBlocks::name() const
{
    return "Variables";
}

void VariableBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "data_variable", &compileVariable);
    engine->addCompileFunction(this, "data_setvariableto", &compileSetVariable);
    engine->addCompileFunction(this, "data_changevariableby", &compileChangeVariableBy);

    // Monitor names
    engine->addMonitorNameFunction(this, "data_variable", &variableMonitorName);

    // Monitor change functions
    engine->addMonitorChangeFunction(this, "data_variable", &changeVariableMonitorValue);

    // Inputs
    engine->addInput(this, "VALUE", VALUE);

    // Fields
    engine->addField(this, "VARIABLE", VARIABLE);
}

void VariableBlocks::compileVariable(Compiler *compiler)
{
    // NOTE: This block is only used by variable monitors
    compiler->addInstruction(vm::OP_READ_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
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

const std::string &VariableBlocks::variableMonitorName(Block *block)
{
    Variable *var = dynamic_cast<Variable *>(block->findFieldById(VARIABLE)->valuePtr().get());

    if (var)
        return var->name();
    else {
        static const std::string empty = "";
        return empty;
    }
}

void VariableBlocks::changeVariableMonitorValue(Block *block, const Value &newValue)
{
    Variable *var = dynamic_cast<Variable *>(block->findFieldById(VARIABLE)->valuePtr().get());

    if (var)
        var->setValue(newValue);
}
