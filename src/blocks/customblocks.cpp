// SPDX-License-Identifier: Apache-2.0

#include "customblocks.h"
#include "../engine/compiler.h"

using namespace libscratchcpp;

CustomBlocks::CustomBlocks()
{
    // Blocks
    addCompileFunction("procedures_definition", &compileDefinition);
    addCompileFunction("procedures_call", &compileCall);
    addCompileFunction("argument_reporter_boolean", &compileArgument);
    addCompileFunction("argument_reporter_string_number", &compileArgument);

    // Inputs
    addInput("custom_block", CUSTOM_BLOCK);

    // Fields
    addField("VALUE", VALUE);
}

std::string CustomBlocks::name() const
{
    return "Custom blocks";
}

void CustomBlocks::compileDefinition(Compiler *compiler)
{
    auto prototype = compiler->input(CUSTOM_BLOCK)->valueBlock()->mutationPrototype();
    compiler->setProcedurePrototype(prototype);
    const std::vector<std::string> &args = prototype->argumentNames();
    const std::string &code = prototype->procCode();
    for (const std::string &arg : args)
        compiler->addProcedureArg(code, arg);
}

void CustomBlocks::compileCall(Compiler *compiler)
{
    compiler->addInstruction(vm::OP_INIT_PROCEDURE);
    auto block = compiler->block();
    auto prototype = block->mutationPrototype();
    const std::vector<std::string> &args = prototype->argumentIds();
    size_t i = 0;
    for (const std::string &id : args) {
        auto index = block->findInput(id);
        if (index == -1)
            compiler->addInstruction(vm::OP_NULL);
        else
            compiler->addInput(block->inputAt(index).get());
        compiler->addInstruction(vm::OP_ADD_ARG);
        i++;
    }
    const std::string &code = prototype->procCode();
    compiler->addInstruction(vm::OP_CALL_PROCEDURE, { compiler->procedureIndex(code) });
}

void CustomBlocks::compileArgument(Compiler *compiler)
{
    if (compiler->procedurePrototype()) {
        const std::string &argName = compiler->field(VALUE)->value().toString();
        auto index = compiler->procedureArgIndex(compiler->procedurePrototype()->procCode(), argName);
        if (index == -1)
            compiler->addInstruction(vm::OP_NULL);
        else
            compiler->addInstruction(vm::OP_READ_ARG, { static_cast<unsigned int>(index) });
    } else
        compiler->addInstruction(vm::OP_NULL);
}
