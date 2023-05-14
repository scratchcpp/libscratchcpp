// SPDX-License-Identifier: Apache-2.0

#include "eventblocks.h"
#include "../engine/compiler.h"
#include "../scratch/broadcast.h"

using namespace libscratchcpp;

EventBlocks::EventBlocks()
{
    // Blocks
    addHatBlock("event_whenflagclicked");
    addCompileFunction("event_broadcast", &compileBroadcast);
    addCompileFunction("event_whenbroadcastreceived", &compileWhenBroadcastReceived);

    // Inputs
    addInput("BROADCAST_INPUT", BROADCAST_INPUT);

    // Fields
    addField("BROADCAST_OPTION", BROADCAST_OPTION);
}

std::string EventBlocks::name() const
{
    return "Events";
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

void EventBlocks::compileWhenBroadcastReceived(Compiler *compiler)
{
    auto broadcast = std::static_pointer_cast<Broadcast>(compiler->field(BROADCAST_OPTION)->valuePtr());
    compiler->engine()->addBroadcastScript(compiler->block(), broadcast);
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
