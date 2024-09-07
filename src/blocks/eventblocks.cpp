// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
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
    engine->addCompileFunction(this, "event_whentouchingobject", &compileWhenTouchingObject);
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
    engine->addHatPredicateCompileFunction(this, "event_whentouchingobject", &compileWhenTouchingObjectPredicate);
    engine->addHatPredicateCompileFunction(this, "event_whengreaterthan", &compileWhenGreaterThanPredicate);

    // Inputs
    engine->addInput(this, "TOUCHINGOBJECTMENU", TOUCHINGOBJECTMENU);
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

void EventBlocks::compileWhenTouchingObjectPredicate(Compiler *compiler)
{
    Input *input = compiler->input(TOUCHINGOBJECTMENU);

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        compiler->addConstValue(value);
        compiler->addFunctionCall(&whenTouchingObjectPredicate);
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&whenTouchingObjectPredicate);
    }
}

void EventBlocks::compileWhenTouchingObject(Compiler *compiler)
{
    compiler->engine()->addWhenTouchingObjectScript(compiler->block());
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
        std::vector<int> broadcasts = compiler->engine()->findBroadcasts(input->primaryValue()->value().toString());

        for (int index : broadcasts) {
            compiler->addConstValue(index);
            compiler->addFunctionCall(&broadcastByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&broadcast);
    }
}

void EventBlocks::compileBroadcastAndWait(Compiler *compiler)
{
    compiler->addInput(BROADCAST_INPUT);
    compiler->addFunctionCall(&broadcastAndWait);
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

unsigned int EventBlocks::whenTouchingObjectPredicate(VirtualMachine *vm)
{
    std::string value = vm->getInput(0, 1)->toString();

    if (value == "_mouse_")
        vm->replaceReturnValue(vm->target()->touchingPoint(vm->engine()->mouseX(), vm->engine()->mouseY()), 1);
    else if (value == "_edge_")
        vm->replaceReturnValue(vm->target()->touchingEdge(), 1);
    else {
        Target *target = vm->engine()->targetAt(vm->engine()->findTarget(value));

        if (target && !target->isStage())
            vm->replaceReturnValue(vm->target()->touchingSprite(static_cast<Sprite *>(target)), 1);
        else
            vm->replaceReturnValue(false, 1);
    }

    return 0;
}

unsigned int EventBlocks::broadcast(VirtualMachine *vm)
{
    std::vector<int> broadcasts = vm->engine()->findBroadcasts(vm->getInput(0, 1)->toString());

    for (int index : broadcasts)
        vm->engine()->broadcast(index, vm);

    return 1;
}

unsigned int EventBlocks::broadcastByIndex(VirtualMachine *vm)
{
    vm->engine()->broadcast(vm->getInput(0, 1)->toLong(), vm);
    return 1;
}

unsigned int EventBlocks::broadcastAndWait(VirtualMachine *vm)
{
    std::vector<int> broadcasts = vm->engine()->findBroadcasts(vm->getInput(0, 1)->toString());

    for (int index : broadcasts)
        vm->engine()->broadcast(index, vm);

    vm->promise();

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
