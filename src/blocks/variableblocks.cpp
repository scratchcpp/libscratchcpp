// SPDX-License-Identifier: Apache-2.0

#include "variableblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

VariableBlocks::VariableBlocks()
{
    // Blocks
    addCompileFunction("data_setvariableto", &VariableBlocks::compileSetVariable);
    addCompileFunction("data_changevariableby", &VariableBlocks::compileChangeVariableBy);
    addBlock("data_setvariableto", &VariableBlocks::setVariable);
    addBlock("data_changevariableby", &VariableBlocks::changeVariableBy);

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

Value VariableBlocks::setVariable(const BlockArgs &args)
{
    auto variable = std::static_pointer_cast<Variable>(args.field(VARIABLE)->valuePtr());
    variable->setValue(args.input(VALUE)->value());
    return Value();
}

Value VariableBlocks::changeVariableBy(const BlockArgs &args)
{
    auto variable = std::static_pointer_cast<Variable>(args.field(VARIABLE)->valuePtr());
    variable->setValue(variable->value() + args.input(VALUE)->value());
    return Value();
}
