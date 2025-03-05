// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/istacktimer.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/value.h>
#include <utf8.h>

#include "looksblocks.h"

using namespace libscratchcpp;

std::string LooksBlocks::name() const
{
    return "Looks";
}

std::string LooksBlocks::description() const
{
    return name() + " blocks";
}

Rgb LooksBlocks::color() const
{
    return rgb(153, 102, 255);
}

void LooksBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "looks_sayforsecs", &compileSayForSecs);
    engine->addCompileFunction(this, "looks_say", &compileSay);
    engine->addCompileFunction(this, "looks_thinkforsecs", &compileThinkForSecs);
}

void LooksBlocks::onInit(IEngine *engine)
{
    engine->threadAboutToStop().connect([](Thread *thread) {
        /*
         * TODO: Scratch uses promises for text bubble timeout
         * which clear the text bubbles even after the thread
         * destroyed. Since we don't use promises, we just
         * clear the text bubble when the thread is destroyed.
         */
        Target *target = thread->target();

        if (target->bubble()->owner() == thread)
            target->bubble()->setText("");
    });

    engine->stopped().connect([engine]() {
        const auto &targets = engine->targets();

        for (auto target : targets) {
            target->bubble()->setText("");
            target->clearGraphicsEffects();
        }
    });
}

void LooksBlocks::compileSayOrThinkForSecs(Compiler *compiler, const std::string function)
{
    auto message = compiler->addInput("MESSAGE");
    auto duration = compiler->addInput("SECS");
    auto saveThread = compiler->addConstValue(true);
    compiler->addFunctionCallWithCtx(function, Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::Bool }, { message, saveThread });
    compiler->addFunctionCallWithCtx("looks_start_stack_timer", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { duration });
    compiler->createYield();

    compiler->beginLoopCondition();
    auto elapsed = compiler->addFunctionCallWithCtx("looks_update_bubble", Compiler::StaticType::Bool);
    compiler->beginRepeatUntilLoop(elapsed);
    compiler->endLoop();
}

CompilerValue *LooksBlocks::compileSayForSecs(Compiler *compiler)
{
    compileSayOrThinkForSecs(compiler, "looks_say");
    return nullptr;
}

CompilerValue *LooksBlocks::compileSay(Compiler *compiler)
{
    auto message = compiler->addInput("MESSAGE");
    auto saveThread = compiler->addConstValue(false);
    compiler->addFunctionCallWithCtx("looks_say", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::Bool }, { message, saveThread });
    return nullptr;
}

CompilerValue *LooksBlocks::compileThinkForSecs(Compiler *compiler)
{
    compileSayOrThinkForSecs(compiler, "looks_think");
    return nullptr;
}

extern "C" void looks_start_stack_timer(ExecutionContext *ctx, double duration)
{
    ctx->stackTimer()->start(duration);
}

void looks_show_bubble(Thread *thread, TextBubble::Type type, const StringPtr *message, bool saveThread)
{
    Target *target = thread->target();
    // TODO: Use UTF-16 for text bubbles
    std::string u8message = utf8::utf16to8(std::u16string(message->data));
    target->bubble()->setType(type);

    if (saveThread)
        target->bubble()->setText(u8message, thread);
    else
        target->bubble()->setText(u8message);
}

extern "C" bool looks_update_bubble(ExecutionContext *ctx)
{
    if (ctx->stackTimer()->elapsed()) {
        Thread *thread = ctx->thread();
        Target *target = thread->target();

        // Clear bubble if it hasn't been changed
        if (target->bubble()->owner() == thread)
            target->bubble()->setText("");

        return true;
    }

    return false;
}

extern "C" void looks_say(ExecutionContext *ctx, const StringPtr *message, bool saveThread)
{
    looks_show_bubble(ctx->thread(), TextBubble::Type::Say, message, saveThread);
}

extern "C" void looks_think(ExecutionContext *ctx, const StringPtr *message, bool saveThread)
{
    looks_show_bubble(ctx->thread(), TextBubble::Type::Think, message, saveThread);
}
