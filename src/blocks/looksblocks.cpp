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
#include <scratchcpp/field.h>
#include <scratchcpp/igraphicseffect.h>
#include <scratchcpp/scratchconfiguration.h>
#include <utf8.h>

#include "looksblocks.h"

using namespace libscratchcpp;

LooksBlocks::~LooksBlocks()
{
    if (m_engine)
        m_instances.erase(m_engine);
}

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
    engine->addCompileFunction(this, "looks_think", &compileThink);
    engine->addCompileFunction(this, "looks_show", &compileShow);
    engine->addCompileFunction(this, "looks_hide", &compileHide);
    engine->addCompileFunction(this, "looks_changeeffectby", &compileChangeEffectBy);
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

    m_engine = engine;
    m_instances[engine] = this;
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

long LooksBlocks::getEffectIndex(IEngine *engine, const std::string &name)
{
    assert(engine);
    assert(m_instances.find(engine) != m_instances.cend());

    LooksBlocks *instance = m_instances[engine];
    auto it = instance->m_effectMap.find(name);

    if (it == instance->m_effectMap.cend()) {
        IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect(name);

        if (effect) {
            instance->m_effects.push_back(effect);
            instance->m_effectMap[name] = instance->m_effects.size() - 1;
            return instance->m_effects.size() - 1;
        } else {
            std::cout << "warning: graphic effect '" << name << "' is not registered" << std::endl;
            return -1;
        }
    }

    return it->second;
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

CompilerValue *LooksBlocks::compileThink(Compiler *compiler)
{
    auto message = compiler->addInput("MESSAGE");
    auto saveThread = compiler->addConstValue(false);
    compiler->addFunctionCallWithCtx("looks_think", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::Bool }, { message, saveThread });
    return nullptr;
}

CompilerValue *LooksBlocks::compileShow(Compiler *compiler)
{
    if (!compiler->target()->isStage())
        compiler->addTargetFunctionCall("looks_show");

    return nullptr;
}

CompilerValue *LooksBlocks::compileHide(Compiler *compiler)
{
    if (!compiler->target()->isStage())
        compiler->addTargetFunctionCall("looks_hide");

    return nullptr;
}

CompilerValue *LooksBlocks::compileChangeEffectBy(Compiler *compiler)
{
    Field *field = compiler->field("EFFECT");

    if (!field)
        return nullptr;

    auto index = getEffectIndex(compiler->engine(), field->value().toString());

    if (index != -1) {
        auto indexValue = compiler->addConstValue(index);
        auto change = compiler->addInput("CHANGE");
        compiler->addTargetFunctionCall("looks_changeeffectby", Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { indexValue, change });
    }

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

extern "C" void looks_show(Sprite *sprite)
{
    sprite->setVisible(true);
}

extern "C" void looks_hide(Sprite *sprite)
{
    sprite->setVisible(false);
}

extern "C" void looks_changeeffectby(Target *target, double index, double change)
{
    IGraphicsEffect *effect = LooksBlocks::getEffect(target->engine(), index);
    target->setGraphicsEffectValue(effect, target->graphicsEffectValue(effect) + change);
}
