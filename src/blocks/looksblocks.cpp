// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/istacktimer.h>
#include <scratchcpp/irandomgenerator.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/value.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/igraphicseffect.h>
#include <scratchcpp/scratchconfiguration.h>
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
    engine->addCompileFunction(this, "looks_think", &compileThink);
    engine->addCompileFunction(this, "looks_show", &compileShow);
    engine->addCompileFunction(this, "looks_hide", &compileHide);
    engine->addCompileFunction(this, "looks_changeeffectby", &compileChangeEffectBy);
    engine->addCompileFunction(this, "looks_seteffectto", &compileSetEffectTo);
    engine->addCompileFunction(this, "looks_cleargraphiceffects", &compileClearGraphicEffects);
    engine->addCompileFunction(this, "looks_changesizeby", &compileChangeSizeBy);
    engine->addCompileFunction(this, "looks_setsizeto", &compileSetSizeTo);
    engine->addCompileFunction(this, "looks_size", &compileSize);
    engine->addCompileFunction(this, "looks_switchcostumeto", &compileSwitchCostumeTo);
    engine->addCompileFunction(this, "looks_nextcostume", &compileNextCostume);
    engine->addCompileFunction(this, "looks_switchbackdropto", &compileSwitchBackdropTo);
    engine->addCompileFunction(this, "looks_gotofrontback", &compileGoToFrontBack);
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

void LooksBlocks::compileSayOrThinkForSecs(Compiler *compiler, const std::string &function)
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

void LooksBlocks::compileSetOrChangeEffect(Compiler *compiler, const std::string &function, const std::string &effectName, CompilerValue *arg)
{
    IGraphicsEffect *effect = ScratchConfiguration::getGraphicsEffect(effectName);

    if (effect) {
        CompilerValue *effectPtr = compiler->addConstValue(effect);
        compiler->addTargetFunctionCall(function, Compiler::StaticType::Void, { Compiler::StaticType::Pointer, Compiler::StaticType::Number }, { effectPtr, arg });
    } else
        std::cout << "warning: graphic effect '" << effectName << "' is not registered" << std::endl;
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

    CompilerValue *change = compiler->addInput("CHANGE");
    compileSetOrChangeEffect(compiler, "looks_changeeffectby", field->value().toString(), change);
    return nullptr;
}

CompilerValue *LooksBlocks::compileSetEffectTo(Compiler *compiler)
{
    Field *field = compiler->field("EFFECT");

    if (!field)
        return nullptr;

    CompilerValue *change = compiler->addInput("VALUE");
    compileSetOrChangeEffect(compiler, "looks_seteffectto", field->value().toString(), change);
    return nullptr;
}

CompilerValue *LooksBlocks::compileClearGraphicEffects(Compiler *compiler)
{
    compiler->addTargetFunctionCall("looks_cleargraphiceffects");
    return nullptr;
}

CompilerValue *LooksBlocks::compileChangeSizeBy(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return nullptr;

    auto change = compiler->addInput("CHANGE");
    compiler->addTargetFunctionCall("looks_changesizeby", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { change });
    return nullptr;
}

CompilerValue *LooksBlocks::compileSetSizeTo(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return nullptr;

    auto size = compiler->addInput("SIZE");
    compiler->addTargetFunctionCall("looks_setsizeto", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { size });
    return nullptr;
}

CompilerValue *LooksBlocks::compileSize(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return compiler->addConstValue(100);
    else
        return compiler->addTargetFunctionCall("looks_size", Compiler::StaticType::Number);
}

CompilerValue *LooksBlocks::compileSwitchCostumeTo(Compiler *compiler)
{
    auto costume = compiler->addInput("COSTUME");
    compiler->addTargetFunctionCall("looks_switchcostumeto", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { costume });
    return nullptr;
}

CompilerValue *LooksBlocks::compileNextCostume(Compiler *compiler)
{
    compiler->addTargetFunctionCall("looks_nextcostume");
    return nullptr;
}

CompilerValue *LooksBlocks::compileSwitchBackdropTo(Compiler *compiler)
{
    auto backdrop = compiler->addInput("BACKDROP");
    auto wait = compiler->addConstValue(false);
    compiler->addFunctionCallWithCtx("looks_switchbackdropto", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { backdrop });
    compiler->addFunctionCallWithCtx("looks_start_backdrop_scripts", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { wait });

    return nullptr;
}

CompilerValue *LooksBlocks::compileGoToFrontBack(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return nullptr;

    Field *field = compiler->field("FRONT_BACK");

    if (!field)
        return nullptr;

    const std::string &option = field->value().toString();

    if (option == "front")
        compiler->addFunctionCallWithCtx("looks_move_to_front");
    else if (option == "back")
        compiler->addFunctionCallWithCtx("looks_move_to_back");

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

extern "C" void looks_changeeffectby(Target *target, IGraphicsEffect *effect, double change)
{
    target->setGraphicsEffectValue(effect, target->graphicsEffectValue(effect) + change);
}

extern "C" void looks_seteffectto(Target *target, IGraphicsEffect *effect, double value)
{
    target->setGraphicsEffectValue(effect, value);
}

extern "C" void looks_cleargraphiceffects(Target *target)
{
    target->clearGraphicsEffects();
}

extern "C" void looks_changesizeby(Sprite *sprite, double change)
{
    sprite->setSize(sprite->size() + change);
}

extern "C" void looks_setsizeto(Sprite *sprite, double size)
{
    sprite->setSize(size);
}

extern "C" double looks_size(Sprite *sprite)
{
    return sprite->size();
}

extern "C" void looks_set_costume_by_index(Target *target, long index)
{
    const size_t costumeCount = target->costumes().size();

    if (index < 0)
        index = (costumeCount + index % (-costumeCount)) % costumeCount;
    else if (index >= costumeCount)
        index = index % costumeCount;

    target->setCostumeIndex(index);
}

extern "C" void looks_nextcostume(Target *target)
{
    looks_set_costume_by_index(target, target->costumeIndex() + 1);
}

extern "C" void looks_previouscostume(Target *target)
{
    looks_set_costume_by_index(target, target->costumeIndex() - 1);
}

void looks_randomcostume(Target *target, IRandomGenerator *rng)
{
    size_t count = target->costumes().size();

    if (count > 1)
        looks_set_costume_by_index(target, rng->randintExcept(0, count - 1, target->costumeIndex())); // exclude current costume
}

extern "C" void looks_switchcostumeto(Target *target, const ValueData *costume)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/blocks/scratch3_looks.js#L389-L413
    if (!value_isString(costume)) {
        // Numbers should be treated as costume indices, always
        if (value_isNaN(costume) || value_isInfinity(costume) || value_isNegativeInfinity(costume))
            target->setCostumeIndex(0);
        else
            looks_set_costume_by_index(target, value_toLong(costume) - 1);
    } else {
        // Strings should be treated as costume names, where possible
        // TODO: Use UTF-16 in Target
        // StringPtr *nameStr = value_toStringPtr(costume);
        std::string nameStr;
        value_toString(costume, &nameStr);
        const int costumeIndex = target->findCostume(nameStr);

        auto it = std::find_if(nameStr.begin(), nameStr.end(), [](char c) { return !std::isspace(c); });
        bool isWhiteSpace = (it == nameStr.end());

        if (costumeIndex != -1)
            looks_set_costume_by_index(target, costumeIndex);
        else if (nameStr == "next costume")
            looks_nextcostume(target);
        else if (nameStr == "previous costume") {
            looks_previouscostume(target);
            // Try to cast the string to a number (and treat it as a costume index)
            // Pure whitespace should not be treated as a number
        } else if (value_isValidNumber(costume) && !isWhiteSpace)
            looks_set_costume_by_index(target, value_toLong(costume) - 1);
    }
}

extern "C" void looks_start_backdrop_scripts(ExecutionContext *ctx, bool wait)
{
    IEngine *engine = ctx->engine();
    Stage *stage = engine->stage();
    Costume *backdrop = stage->currentCostume().get();

    if (backdrop)
        engine->startBackdropScripts(backdrop->broadcast(), ctx->thread(), wait);
}

extern "C" void looks_switchbackdropto(ExecutionContext *ctx, const ValueData *backdrop)
{
    Stage *stage = ctx->engine()->stage();

    // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/blocks/scratch3_looks.js#L389-L413
    if (!value_isString(backdrop)) {
        // Numbers should be treated as costume indices, always
        if (value_isNaN(backdrop) || value_isInfinity(backdrop) || value_isNegativeInfinity(backdrop))
            stage->setCostumeIndex(0);
        else
            looks_set_costume_by_index(stage, value_toLong(backdrop) - 1);
    } else {
        // Strings should be treated as costume names, where possible
        // TODO: Use UTF-16 in Target
        // StringPtr *nameStr = value_toStringPtr(backdrop);
        std::string nameStr;
        value_toString(backdrop, &nameStr);
        const int costumeIndex = stage->findCostume(nameStr);

        auto it = std::find_if(nameStr.begin(), nameStr.end(), [](char c) { return !std::isspace(c); });
        bool isWhiteSpace = (it == nameStr.end());

        if (costumeIndex != -1)
            looks_set_costume_by_index(stage, costumeIndex);
        else if (nameStr == "next backdrop")
            looks_nextcostume(stage);
        else if (nameStr == "previous backdrop")
            looks_previouscostume(stage);
        else if (nameStr == "random backdrop") {
            looks_randomcostume(stage, ctx->rng());
            // Try to cast the string to a number (and treat it as a costume index)
            // Pure whitespace should not be treated as a number
        } else if (value_isValidNumber(backdrop) && !isWhiteSpace)
            looks_set_costume_by_index(stage, value_toLong(backdrop) - 1);
    }
}

extern "C" void looks_move_to_front(ExecutionContext *ctx)
{
    Target *target = ctx->thread()->target();
    ctx->engine()->moveDrawableToFront(target);
}

extern "C" void looks_move_to_back(ExecutionContext *ctx)
{
    Target *target = ctx->thread()->target();
    ctx->engine()->moveDrawableToBack(target);
}
