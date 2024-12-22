// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>

#include "operatorblocks.h"

using namespace libscratchcpp;

std::string OperatorBlocks::name() const
{
    return "Operators";
}

std::string OperatorBlocks::description() const
{
    return "Operator blocks";
}

void OperatorBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "operator_add", &compileAdd);
    engine->addCompileFunction(this, "operator_subtract", &compileSubtract);
}

CompilerValue *OperatorBlocks::compileAdd(Compiler *compiler)
{
    return compiler->createAdd(compiler->addInput("NUM1"), compiler->addInput("NUM2"));
}

CompilerValue *OperatorBlocks::compileSubtract(Compiler *compiler)
{
    return compiler->createSub(compiler->addInput("NUM1"), compiler->addInput("NUM2"));
}
