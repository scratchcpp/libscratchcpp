// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/sprite.h>
#include <utf8.h>

#include "eventblocks.h"

using namespace libscratchcpp;

std::string EventBlocks::name() const
{
    return "Events";
}

std::string libscratchcpp::EventBlocks::description() const
{
    return "Event blocks";
}

Rgb EventBlocks::color() const
{
    return rgb(255, 191, 0);
}

void EventBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "event_whentouchingobject", &compileWhenTouchingObject);
    engine->addCompileFunction(this, "event_whenflagclicked", &compileWhenFlagClicked);
    engine->addCompileFunction(this, "event_whenthisspriteclicked", &compileWhenThisSpriteClicked);
    engine->addCompileFunction(this, "event_whenstageclicked", &compileWhenStageClicked);
    engine->addCompileFunction(this, "event_whenbroadcastreceived", &compileWhenBroadcastReceived);
    engine->addCompileFunction(this, "event_whenbackdropswitchesto", &compileWhenBackdropSwitchesTo);
    engine->addCompileFunction(this, "event_whengreaterthan", &compileWhenGreaterThan);
    engine->addCompileFunction(this, "event_broadcast", &compileBroadcast);
    engine->addCompileFunction(this, "event_broadcastandwait", &compileBroadcastAndWait);
    engine->addCompileFunction(this, "event_whenkeypressed", &compileWhenKeyPressed);

    // Hat predicates
    engine->addHatPredicateCompileFunction(this, "event_whentouchingobject", &compileWhenTouchingObjectPredicate);
}

CompilerValue *EventBlocks::compileWhenTouchingObject(Compiler *compiler)
{
    compiler->engine()->addWhenTouchingObjectScript(compiler->block());
    return nullptr;
}

CompilerValue *EventBlocks::compileWhenTouchingObjectPredicate(Compiler *compiler)
{
    Input *input = compiler->input("TOUCHINGOBJECTMENU");
    CompilerValue *name = nullptr;

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        name = compiler->addConstValue(value);
    } else
        name = compiler->addInput(input);

    return compiler->addTargetFunctionCall("event_whentouchingobject_predicate", Compiler::StaticType::Bool, { Compiler::StaticType::String }, { name });
}

CompilerValue *EventBlocks::compileWhenFlagClicked(Compiler *compiler)
{
    compiler->engine()->addGreenFlagScript(compiler->block());
    return nullptr;
}

CompilerValue *EventBlocks::compileWhenThisSpriteClicked(Compiler *compiler)
{
    compiler->engine()->addTargetClickScript(compiler->block());
    return nullptr;
}

CompilerValue *EventBlocks::compileWhenStageClicked(Compiler *compiler)
{
    compiler->engine()->addTargetClickScript(compiler->block());
    return nullptr;
}

CompilerValue *EventBlocks::compileWhenBroadcastReceived(Compiler *compiler)
{
    auto block = compiler->block();
    Field *field = compiler->field("BROADCAST_OPTION");

    if (field) {
        auto broadcast = std::static_pointer_cast<Broadcast>(field->valuePtr());
        compiler->engine()->addBroadcastScript(block, field, broadcast.get());
    }

    return nullptr;
}

CompilerValue *EventBlocks::compileWhenBackdropSwitchesTo(Compiler *compiler)
{
    auto block = compiler->block();
    Field *field = compiler->field("BACKDROP");

    if (field)
        compiler->engine()->addBackdropChangeScript(block, field);

    return nullptr;
}

CompilerValue *EventBlocks::compileWhenGreaterThan(Compiler *compiler)
{
    compiler->engine()->addWhenGreaterThanScript(compiler->block());
    return nullptr;
}

CompilerValue *EventBlocks::compileBroadcast(Compiler *compiler)
{
    auto input = compiler->addInput("BROADCAST_INPUT");
    auto wait = compiler->addConstValue(false);
    compiler->addFunctionCallWithCtx("event_broadcast", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::Bool }, { input, wait });
    return nullptr;
}

CompilerValue *EventBlocks::compileBroadcastAndWait(Compiler *compiler)
{
    auto input = compiler->addInput("BROADCAST_INPUT");
    auto wait = compiler->addConstValue(true);
    compiler->addFunctionCallWithCtx("event_broadcast", Compiler::StaticType::Void, { Compiler::StaticType::String, Compiler::StaticType::Bool }, { input, wait });
    compiler->createYield();
    return nullptr;
}

CompilerValue *EventBlocks::compileWhenKeyPressed(Compiler *compiler)
{
    auto block = compiler->block();
    Field *field = compiler->field("KEY_OPTION");

    if (field)
        compiler->engine()->addKeyPressScript(block, field);

    return nullptr;
}

extern "C" bool event_whentouchingobject_predicate(Target *target, const StringPtr *name)
{
    static const StringPtr MOUSE_STR = StringPtr("_mouse_");
    static const StringPtr EDGE_STR = StringPtr("_edge_");

    IEngine *engine = target->engine();

    if (string_compare_case_sensitive(name, &MOUSE_STR) == 0)
        return target->touchingPoint(engine->mouseX(), engine->mouseY());
    else if (string_compare_case_sensitive(name, &EDGE_STR) == 0)
        return target->touchingEdge();
    else {
        // TODO: Use UTF-16 in engine
        const std::string u8name = utf8::utf16to8(std::u16string(name->data));
        Target *anotherTarget = engine->targetAt(engine->findTarget(u8name));

        if (anotherTarget && !anotherTarget->isStage())
            return target->touchingSprite(static_cast<Sprite *>(anotherTarget));
        else
            return false;
    }
}

extern "C" void event_broadcast(ExecutionContext *ctx, const StringPtr *name, bool wait)
{
    Thread *thread = ctx->thread();
    IEngine *engine = thread->engine();
    // TODO: Use UTF-16 in engine
    std::vector<int> broadcasts = engine->findBroadcasts(utf8::utf16to8(std::u16string(name->data)));

    for (int index : broadcasts)
        engine->broadcast(index, thread, wait);

    if (wait)
        ctx->setPromise(std::make_shared<Promise>());
}
