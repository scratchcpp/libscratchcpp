// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/value.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/istacktimer.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>
#include <utf8.h>

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

Rgb ControlBlocks::color() const
{
    return rgb(255, 171, 25);
}

void ControlBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "control_forever", &compileForever);
    engine->addCompileFunction(this, "control_repeat", &compileRepeat);
    engine->addCompileFunction(this, "control_if", &compileIf);
    engine->addCompileFunction(this, "control_if_else", &compileIfElse);
    engine->addCompileFunction(this, "control_stop", &compileStop);
    engine->addCompileFunction(this, "control_wait", &compileWait);
    engine->addCompileFunction(this, "control_wait_until", &compileWaitUntil);
    engine->addCompileFunction(this, "control_repeat_until", &compileRepeatUntil);
    engine->addCompileFunction(this, "control_while", &compileWhile);
    engine->addCompileFunction(this, "control_for_each", &compileForEach);
    engine->addCompileFunction(this, "control_start_as_clone", &compileStartAsClone);
    engine->addCompileFunction(this, "control_create_clone_of", &compileCreateCloneOf);
    engine->addCompileFunction(this, "control_delete_this_clone", &compileDeleteThisClone);
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

CompilerValue *ControlBlocks::compileWait(Compiler *compiler)
{
    auto duration = compiler->addInput("DURATION");
    compiler->addFunctionCallWithCtx("control_start_wait", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { duration });
    compiler->createYield();

    compiler->beginLoopCondition();
    auto elapsed = compiler->addFunctionCallWithCtx("control_stack_timer_elapsed", Compiler::StaticType::Bool);
    compiler->beginRepeatUntilLoop(elapsed);
    compiler->endLoop();

    return nullptr;
}

CompilerValue *ControlBlocks::compileWaitUntil(Compiler *compiler)
{
    compiler->beginLoopCondition();
    compiler->beginRepeatUntilLoop(compiler->addInput("CONDITION"));
    compiler->endLoop();
    return nullptr;
}

CompilerValue *ControlBlocks::compileRepeatUntil(Compiler *compiler)
{
    auto substack = compiler->input("SUBSTACK");
    compiler->beginLoopCondition();
    compiler->moveToRepeatUntilLoop(compiler->addInput("CONDITION"), substack ? substack->valueBlock() : nullptr);
    return nullptr;
}

CompilerValue *ControlBlocks::compileWhile(Compiler *compiler)
{
    auto substack = compiler->input("SUBSTACK");
    compiler->beginLoopCondition();
    compiler->moveToWhileLoop(compiler->addInput("CONDITION"), substack ? substack->valueBlock() : nullptr);
    return nullptr;
}

CompilerValue *ControlBlocks::compileForEach(Compiler *compiler)
{
    Variable *var = static_cast<Variable *>(compiler->field("VARIABLE")->valuePtr().get());
    assert(var);
    auto substack = compiler->input("SUBSTACK");
    compiler->moveToRepeatLoop(compiler->addInput("VALUE"), substack ? substack->valueBlock() : nullptr);
    auto index = compiler->createAdd(compiler->addLoopIndex(), compiler->addConstValue(1));
    compiler->createVariableWrite(var, index);
    return nullptr;
}

CompilerValue *ControlBlocks::compileStartAsClone(Compiler *compiler)
{
    compiler->engine()->addCloneInitScript(compiler->block());
    return nullptr;
}

CompilerValue *ControlBlocks::compileCreateCloneOf(Compiler *compiler)
{
    Input *input = compiler->input("CLONE_OPTION");

    if (input->pointsToDropdownMenu()) {
        std::string spriteName = input->selectedMenuItem();

        if (spriteName == "_myself_")
            compiler->addTargetFunctionCall("control_create_clone_of_myself");
        else {
            auto index = compiler->engine()->findTarget(spriteName);
            CompilerValue *arg = compiler->addConstValue(index);
            compiler->addFunctionCallWithCtx("control_create_clone_by_index", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { arg });
        }
    } else {
        CompilerValue *arg = compiler->addInput("CLONE_OPTION");
        compiler->addFunctionCallWithCtx("control_create_clone", Compiler::StaticType::Void, { Compiler::StaticType::String }, { arg });
    }

    return nullptr;
}

CompilerValue *ControlBlocks::compileDeleteThisClone(Compiler *compiler)
{
    CompilerValue *deleted = compiler->addTargetFunctionCall("control_delete_this_clone", Compiler::StaticType::Bool);
    compiler->beginIfStatement(deleted);
    compiler->createStopWithoutSync(); // sync happens before the function call
    compiler->endIf();
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

extern "C" void control_start_wait(ExecutionContext *ctx, double seconds)
{
    ctx->stackTimer()->start(seconds);
    ctx->engine()->requestRedraw();
}

extern "C" bool control_stack_timer_elapsed(ExecutionContext *ctx)
{
    return ctx->stackTimer()->elapsed();
}

extern "C" void control_create_clone_of_myself(Target *target)
{
    if (!target->isStage())
        static_cast<Sprite *>(target)->clone();
}

extern "C" void control_create_clone_by_index(ExecutionContext *ctx, double index)
{
    Target *target = ctx->engine()->targetAt(index);

    if (!target->isStage())
        static_cast<Sprite *>(target)->clone();
}

extern "C" void control_create_clone(ExecutionContext *ctx, const StringPtr *spriteName)
{
    static const StringPtr myself("_myself_");
    if (string_compare_case_sensitive(spriteName, &myself) == 0)
        control_create_clone_of_myself(ctx->thread()->target());
    else {
        IEngine *engine = ctx->engine();
        // TODO: Use UTF-16 in engine
        auto index = engine->findTarget(utf8::utf16to8(std::u16string(spriteName->data)));
        Target *target = engine->targetAt(index);

        if (!target->isStage())
            static_cast<Sprite *>(target)->clone();
    }
}

extern "C" bool control_delete_this_clone(Target *target)
{
    if (!target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(target);

        if (sprite->isClone()) {
            target->engine()->stopTarget(target, nullptr);
            sprite->deleteClone();
            return true;
        }
    }

    return false;
}
