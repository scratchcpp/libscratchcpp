// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/block.h>
#include <scratchcpp/field.h>

#include "customblocks.h"

using namespace libscratchcpp;

std::string CustomBlocks::name() const
{
    return "Custom blocks";
}

std::string CustomBlocks::description() const
{
    return name();
}

Rgb CustomBlocks::color() const
{
    return rgb(255, 102, 128);
}

void CustomBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "procedures_definition", [](Compiler *) -> CompilerValue * { return nullptr; });
    engine->addCompileFunction(this, "procedures_call", &compileCall);
    engine->addCompileFunction(this, "argument_reporter_boolean", &compileArgument);
    engine->addCompileFunction(this, "argument_reporter_string_number", &compileArgument);
}

CompilerValue *CustomBlocks::compileCall(Compiler *compiler)
{
    auto block = compiler->block();
    auto prototype = block->mutationPrototype();
    const std::vector<std::string> &procedureArgs = prototype->argumentIds();
    Compiler::Args args;

    for (size_t i = 0; i < procedureArgs.size(); i++) {
        auto index = block->findInput(procedureArgs[i]);

        if (index == -1)
            args.push_back(compiler->addConstValue(Value()));
        else
            args.push_back(compiler->addInput(block->inputAt(index).get()));
    }

    compiler->createProcedureCall(compiler->block()->mutationPrototype(), args);
    return nullptr;
}

CompilerValue *CustomBlocks::compileArgument(Compiler *compiler)
{
    const std::string &argName = compiler->field("VALUE")->value().toString();
    return compiler->addProcedureArgument(argName);
}
