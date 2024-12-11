// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/value.h>
#include <scratchcpp/input.h>

#include "controlblocks.h"

using namespace libscratchcpp;

std::string ControlBlocks::name() const
{
    return "Control";
}

std::string ControlBlocks::description() const
{
    return name() + " blocks";
}

void ControlBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "control_forever", &compileForever);
    engine->addCompileFunction(this, "control_repeat", &compileRepeat);
    engine->addCompileFunction(this, "control_if", &compileIf);
    engine->addCompileFunction(this, "control_if_else", &compileIfElse);
}

CompilerValue *ControlBlocks::compileForever(Compiler *compiler)
{
    auto substack = compiler->input("SUBSTACK");
    compiler->beginLoopCondition();
    compiler->moveToWhileLoop(compiler->addConstValue(true), substack ? substack->valueBlock() : nullptr);
    return nullptr;
}

CompilerValue *ControlBlocks::compileRepeat(Compiler *compiler)
{
    auto substack = compiler->input("SUBSTACK");
    compiler->moveToRepeatLoop(compiler->addInput("TIMES"), substack ? substack->valueBlock() : nullptr);
    return nullptr;
}

CompilerValue *ControlBlocks::compileIf(Compiler *compiler)
{
    auto substack = compiler->input("SUBSTACK");
    compiler->moveToIf(compiler->addInput("CONDITION"), substack ? substack->valueBlock() : nullptr);
    return nullptr;
}

CompilerValue *ControlBlocks::compileIfElse(Compiler *compiler)
{
    auto substack = compiler->input("SUBSTACK");
    auto substack2 = compiler->input("SUBSTACK2");
    compiler->moveToIfElse(compiler->addInput("CONDITION"), substack ? substack->valueBlock() : nullptr, substack2 ? substack2->valueBlock() : nullptr);
    return nullptr;
}
