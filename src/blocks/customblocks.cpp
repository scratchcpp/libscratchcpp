// SPDX-License-Identifier: Apache-2.0

#include "customblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

CustomBlocks::CustomBlocks()
{
    // Blocks
    addHatBlock("procedures_definition");
    addCompileFunction("procedures_call", &compileCall);
}

std::string CustomBlocks::name() const
{
    return "Custom blocks";
}

void CustomBlocks::compileCall(Compiler *compiler)
{
    const std::string &code = compiler->block()->mutationPrototype()->procCode();
    compiler->addInstruction(vm::OP_CALL_PROCEDURE, { compiler->procedureIndex(code) });
}
