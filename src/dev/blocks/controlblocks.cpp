// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/value.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/thread.h>

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
    engine->addCompileFunction(this, "control_stop", &compileStop);
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

CompilerValue *ControlBlocks::compileStop(Compiler *compiler)
{
    Field *option = compiler->field("STOP_OPTION");

    if (option) {
        std::string str = option->value().toString();

        if (str == "all")
            compiler->addFunctionCallWithCtx("control_stop_all", Compiler::StaticType::Void);
        else if (str == "this script")
            compiler->createStop();
        else if (str == "other scripts in sprite" || str == "other scripts in stage")
            compiler->addFunctionCallWithCtx("control_stop_other_scripts_in_target", Compiler::StaticType::Void);
    }

    return nullptr;
}

extern "C" void control_stop_all(ExecutionContext *ctx)
{
    ctx->engine()->stop();
}

extern "C" void control_stop_other_scripts_in_target(ExecutionContext *ctx)
{
    Thread *thread = ctx->thread();
    ctx->engine()->stopTarget(thread->target(), thread);
}
