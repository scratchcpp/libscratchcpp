// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>

#include "eventblocks.h"

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
    engine->addCompileFunction(this, "event_whenbackdropswitchesto", &compileWhenBackdropSwitchesTo);
    engine->addCompileFunction(this, "event_whenkeypressed", &compileWhenKeyPressed);

    // Inputs
    engine->addInput(this, "BROADCAST_INPUT", BROADCAST_INPUT);

    // Fields
    engine->addField(this, "BROADCAST_OPTION", BROADCAST_OPTION);
    engine->addField(this, "BACKDROP", BACKDROP);
    engine->addField(this, "KEY_OPTION", KEY_OPTION);
}

void EventBlocks::compileBroadcast(Compiler *compiler)
{
    auto input = compiler->input(BROADCAST_INPUT);

    if (input->type() != Input::Type::ObscuredShadow) {
        compiler->addConstValue(compiler->engine()->findBroadcast(input->primaryValue()->value().toString()));
        compiler->addFunctionCall(&broadcastByIndex);
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&broadcast);
    }
}

void EventBlocks::compileBroadcastAndWait(Compiler *compiler)
{
    auto input = compiler->input(BROADCAST_INPUT);

    if (input->type() != Input::Type::ObscuredShadow) {
        int index = compiler->engine()->findBroadcast(input->primaryValue()->value().toString());
        compiler->addConstValue(index);
        compiler->addFunctionCall(&broadcastByIndexAndWait);
        compiler->addConstValue(index);
        compiler->addFunctionCall(&checkBroadcastByIndex);
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&broadcastAndWait);
        compiler->addInput(input);
        compiler->addFunctionCall(&checkBroadcast);
    }
}

void EventBlocks::compileWhenBroadcastReceived(Compiler *compiler)
{
    auto broadcast = std::static_pointer_cast<Broadcast>(compiler->field(BROADCAST_OPTION)->valuePtr());

    compiler->engine()->addBroadcastScript(compiler->block(), broadcast.get());
}

void EventBlocks::compileWhenBackdropSwitchesTo(Compiler *compiler)
{
    if (Stage *stage = compiler->engine()->stage()) {
        std::string backdropName = compiler->field(BACKDROP)->value().toString();
        int index = stage->findCostume(backdropName);

        if (index != -1)
            compiler->engine()->addBroadcastScript(compiler->block(), stage->costumeAt(index)->broadcast());
    }
}

void EventBlocks::compileWhenKeyPressed(Compiler *compiler)
{
    // NOTE: Field values don't have to be registered because keys are referenced by their names
    compiler->engine()->addKeyPressScript(compiler->block(), compiler->field(KEY_OPTION)->value().toString());
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

unsigned int EventBlocks::broadcastAndWait(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->engine()->findBroadcast(vm->getInput(0, 1)->toString()), vm, true);
    return 1;
}

unsigned int EventBlocks::broadcastByIndexAndWait(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->getInput(0, 1)->toLong(), vm, true);
    return 1;
}

unsigned int EventBlocks::checkBroadcast(VirtualMachine *vm)
{
    if (vm->engine()->broadcastRunning(vm->engine()->findBroadcast(vm->getInput(0, 1)->toString()), vm))
        vm->stop(true, true, true);
    return 1;
}

unsigned int EventBlocks::checkBroadcastByIndex(VirtualMachine *vm)
{
    if (vm->engine()->broadcastRunning(vm->getInput(0, 1)->toLong(), vm))
        vm->stop(true, true, true);
    return 1;
}
