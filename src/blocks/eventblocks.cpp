// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/compiler.h>

#include "eventblocks.h"

// TODO: Remove this
#include "engine/engine.h"

using namespace libscratchcpp;

std::string EventBlocks::name() const
{
    return "Events";
}

void EventBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addHatBlock(this, "event_whenflagclicked");
    engine->addCompileFunction(this, "event_broadcast", &compileBroadcast);
    engine->addCompileFunction(this, "event_broadcastandwait", &compileBroadcastAndWait);
    engine->addCompileFunction(this, "event_whenbroadcastreceived", &compileWhenBroadcastReceived);

    // Inputs
    engine->addInput(this, "BROADCAST_INPUT", BROADCAST_INPUT);

    // Fields
    engine->addField(this, "BROADCAST_OPTION", BROADCAST_OPTION);
}

void EventBlocks::compileBroadcast(Compiler *compiler)
{
    auto input = compiler->input(BROADCAST_INPUT);
    compiler->addInput(input);
    if (input->type() != Input::Type::ObscuredShadow) {
        input->primaryValue()->setValue(compiler->engine()->findBroadcast(input->primaryValue()->value().toString()));
        compiler->addFunctionCall(&broadcastByIndex);
    } else
        compiler->addFunctionCall(&broadcast);
}

void EventBlocks::compileBroadcastAndWait(Compiler *compiler)
{
    auto input = compiler->input(BROADCAST_INPUT);
    compiler->addInput(input);
    if (input->type() != Input::Type::ObscuredShadow) {
        input->primaryValue()->setValue(compiler->engine()->findBroadcast(input->primaryValue()->value().toString()));
        compiler->addFunctionCall(&broadcastByIndex);
        compiler->addInput(input);
        compiler->addFunctionCall(&checkBroadcastByIndex);
    } else {
        compiler->addFunctionCall(&broadcast);
        compiler->addInput(input);
        compiler->addFunctionCall(&checkBroadcast);
    }
}

void EventBlocks::compileWhenBroadcastReceived(Compiler *compiler)
{
    auto broadcast = std::static_pointer_cast<Broadcast>(compiler->field(BROADCAST_OPTION)->valuePtr());

    // TODO: Remove the cast
    dynamic_cast<Engine *>(compiler->engine())->addBroadcastScript(compiler->block(), broadcast);
}

unsigned int EventBlocks::broadcast(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->engine()->findBroadcast(vm->getInput(0, 1)->toString()), vm);
    return 1;
}

unsigned int EventBlocks::broadcastByIndex(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->getInput(0, 1)->toLong(), vm);
    return 1;
}

unsigned int EventBlocks::checkBroadcast(VirtualMachine *vm)
{
    if (vm->engine()->broadcastRunning(vm->engine()->findBroadcast(vm->getInput(0, 1)->toString())))
        vm->stop(true, true, true);
    return 1;
}

unsigned int EventBlocks::checkBroadcastByIndex(VirtualMachine *vm)
{
    if (vm->engine()->broadcastRunning(vm->getInput(0, 1)->toLong()))
        vm->stop(true, true, true);
    return 1;
}
