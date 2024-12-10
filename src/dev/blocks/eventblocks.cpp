// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/field.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/dev/compilerconstant.h>

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

void EventBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "event_whentouchingobject", &compileWhenTouchingObject);
    engine->addCompileFunction(this, "event_whenflagclicked", &compileWhenFlagClicked);
    engine->addCompileFunction(this, "event_whenthisspriteclicked", &compileWhenThisSpriteClicked);
    engine->addCompileFunction(this, "event_whenstageclicked", &compileWhenStageClicked);
    engine->addCompileFunction(this, "event_whenbroadcastreceived", &compileWhenBroadcastReceived);
    engine->addCompileFunction(this, "event_whenbackdropswitchesto", &compileWhenBackdropSwitchesTo);
    engine->addCompileFunction(this, "event_whengreaterthan", &compileWhenGreaterThan);
    engine->addCompileFunction(this, "event_broadcast", &compileBroadcast);
    engine->addCompileFunction(this, "event_broadcastandwait", &compileBroadcastAndWait);
}

CompilerValue *EventBlocks::compileWhenTouchingObject(Compiler *compiler)
{
    compiler->engine()->addWhenTouchingObjectScript(compiler->block());
    return nullptr;
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
    return nullptr;
}

extern "C" void event_broadcast(ExecutionContext *ctx, const char *name, bool wait)
{
    Thread *thread = ctx->thread();
    IEngine *engine = thread->engine();
    std::vector<int> broadcasts = engine->findBroadcasts(name);

    for (int index : broadcasts)
        engine->broadcast(index, thread, wait);
}
