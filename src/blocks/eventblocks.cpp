// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/block.h>
#include <scratchcpp/itimer.h>

#include "eventblocks.h"
#include "audio/audioinput.h"
#include "audio/iaudioloudness.h"

using namespace libscratchcpp;

IAudioInput *EventBlocks::audioInput = nullptr;

std::string EventBlocks::name() const
{
    return "Events";
}

void EventBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "event_whenflagclicked", &compileWhenFlagClicked);
    engine->addCompileFunction(this, "event_whenthisspriteclicked", &compileWhenThisSpriteClicked);
    engine->addCompileFunction(this, "event_whenstageclicked", &compileWhenStageClicked);
    engine->addCompileFunction(this, "event_broadcast", &compileBroadcast);
    engine->addCompileFunction(this, "event_broadcastandwait", &compileBroadcastAndWait);
    engine->addCompileFunction(this, "event_whenbroadcastreceived", &compileWhenBroadcastReceived);
    engine->addCompileFunction(this, "event_whenbackdropswitchesto", &compileWhenBackdropSwitchesTo);
    engine->addCompileFunction(this, "event_whengreaterthan", &compileWhenGreaterThan);
    engine->addCompileFunction(this, "event_whenkeypressed", &compileWhenKeyPressed);

    // Hat predicates
    engine->addHatPredicateCompileFunction(this, "event_whengreaterthan", &compileWhenGreaterThanPredicate);

    // Inputs
    engine->addInput(this, "BROADCAST_INPUT", BROADCAST_INPUT);
    engine->addInput(this, "VALUE", VALUE);

    // Fields
    engine->addField(this, "BROADCAST_OPTION", BROADCAST_OPTION);
    engine->addField(this, "BACKDROP", BACKDROP);
    engine->addField(this, "WHENGREATERTHANMENU", WHENGREATERTHANMENU);
    engine->addField(this, "KEY_OPTION", KEY_OPTION);

    // Fields values
    engine->addFieldValue(this, "LOUDNESS", Loudness);
    engine->addFieldValue(this, "TIMER", Timer);
}

void EventBlocks::compileWhenFlagClicked(Compiler *compiler)
{
    compiler->engine()->addGreenFlagScript(compiler->block());
}

void EventBlocks::compileWhenThisSpriteClicked(Compiler *compiler)
{
    compiler->engine()->addTargetClickScript(compiler->block());
}

void EventBlocks::compileWhenStageClicked(Compiler *compiler)
{
    compiler->engine()->addTargetClickScript(compiler->block());
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

    compiler->engine()->addBroadcastScript(compiler->block(), BROADCAST_OPTION, broadcast.get());
}

void EventBlocks::compileWhenBackdropSwitchesTo(Compiler *compiler)
{
    compiler->engine()->addBackdropChangeScript(compiler->block(), BACKDROP);
}

void EventBlocks::compileWhenGreaterThanPredicate(Compiler *compiler)
{
    Field *field = compiler->field(WHENGREATERTHANMENU);
    BlockFunc predicate = nullptr;

    if (field) {
        switch (field->specialValueId()) {
            case Loudness:
                predicate = &whenLoudnessGreaterThanPredicate;
                break;

            case Timer:
                predicate = &whenTimerGreaterThanPredicate;
                break;

            default:
                compiler->addInstruction(vm::OP_NULL);
                return;
        }
    }

    if (predicate) {
        compiler->addInput(VALUE);
        compiler->addFunctionCall(predicate);
    }
}

void EventBlocks::compileWhenGreaterThan(Compiler *compiler)
{
    compiler->engine()->addWhenGreaterThanScript(compiler->block());
}

void EventBlocks::compileWhenKeyPressed(Compiler *compiler)
{
    // NOTE: Field values don't have to be registered because keys are referenced by their names
    compiler->engine()->addKeyPressScript(compiler->block(), KEY_OPTION);
}

unsigned int EventBlocks::broadcast(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->engine()->findBroadcast(vm->getInput(0, 1)->toString()));
    return 1;
}

unsigned int EventBlocks::broadcastByIndex(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->getInput(0, 1)->toLong());
    return 1;
}

unsigned int EventBlocks::broadcastAndWait(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->engine()->findBroadcast(vm->getInput(0, 1)->toString()));
    return 1;
}

unsigned int EventBlocks::broadcastByIndexAndWait(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->getInput(0, 1)->toLong());
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

unsigned int EventBlocks::whenLoudnessGreaterThanPredicate(VirtualMachine *vm)
{
    if (!audioInput)
        audioInput = AudioInput::instance().get();

    auto audioLoudness = audioInput->audioLoudness();
    const Value &operand = *vm->getInput(0, 1);
    vm->replaceReturnValue(Value(audioLoudness->getLoudness()) > operand, 1);
    return 0;
}

unsigned int EventBlocks::whenTimerGreaterThanPredicate(VirtualMachine *vm)
{
    const Value &operand = *vm->getInput(0, 1);
    vm->replaceReturnValue(Value(vm->engine()->timer()->value()) > operand, 1);
    return 0;
}
