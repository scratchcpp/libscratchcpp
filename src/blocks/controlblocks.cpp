// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/block.h>
#include <cassert>

#include "controlblocks.h"
#include "../engine/internal/clock.h"

using namespace libscratchcpp;

IClock *ControlBlocks::clock = nullptr;

std::string ControlBlocks::name() const
{
    return "Control";
}

std::string ControlBlocks::description() const
{
    return name() + " blocks";
}

void ControlBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "control_forever", &compileRepeatForever);
    engine->addCompileFunction(this, "control_repeat", &compileRepeat);
    engine->addCompileFunction(this, "control_repeat_until", &compileRepeatUntil);
    engine->addCompileFunction(this, "control_while", &compileRepeatWhile);
    engine->addCompileFunction(this, "control_for_each", &compileRepeatForEach);
    engine->addCompileFunction(this, "control_if", &compileIfStatement);
    engine->addCompileFunction(this, "control_if_else", &compileIfElseStatement);
    engine->addCompileFunction(this, "control_stop", &compileStop);
    engine->addCompileFunction(this, "control_wait", &compileWait);
    engine->addCompileFunction(this, "control_wait_until", &compileWaitUntil);
    engine->addCompileFunction(this, "control_start_as_clone", &compileStartAsClone);
    engine->addCompileFunction(this, "control_create_clone_of", &compileCreateClone);
    engine->addCompileFunction(this, "control_delete_this_clone", &compileDeleteThisClone);

    // Inputs
    engine->addInput(this, "SUBSTACK", SUBSTACK);
    engine->addInput(this, "SUBSTACK2", SUBSTACK2);
    engine->addInput(this, "TIMES", TIMES);
    engine->addInput(this, "CONDITION", CONDITION);
    engine->addInput(this, "DURATION", DURATION);
    engine->addInput(this, "VALUE", VALUE);
    engine->addInput(this, "CLONE_OPTION", CLONE_OPTION);

    // Fields
    engine->addField(this, "STOP_OPTION", STOP_OPTION);
    engine->addField(this, "VARIABLE", VARIABLE);

    // Field values
    engine->addFieldValue(this, "all", StopAll);
    engine->addFieldValue(this, "this script", StopThisScript);
    engine->addFieldValue(this, "other scripts in sprite", StopOtherScriptsInSprite);
    engine->addFieldValue(this, "other scripts in stage", StopOtherScriptsInSprite);
}

void ControlBlocks::compileRepeatForever(Compiler *compiler)
{
    compiler->addInstruction(vm::OP_FOREVER_LOOP);
    compiler->moveToSubstack(compiler->inputBlock(SUBSTACK), Compiler::SubstackType::Loop);
}

void ControlBlocks::compileRepeat(Compiler *compiler)
{
    auto substack = compiler->inputBlock(SUBSTACK);
    if (substack) {
        compiler->addInput(TIMES);
        compiler->addInstruction(vm::OP_REPEAT_LOOP);
        compiler->moveToSubstack(substack, Compiler::SubstackType::Loop);
    }
}

void ControlBlocks::compileRepeatUntil(Compiler *compiler)
{
    auto substack = compiler->inputBlock(SUBSTACK);
    compiler->addInstruction(vm::OP_UNTIL_LOOP);
    compiler->addInput(CONDITION);
    compiler->addInstruction(vm::OP_BEGIN_UNTIL_LOOP);
    compiler->moveToSubstack(substack, Compiler::SubstackType::Loop);
}

void ControlBlocks::compileRepeatWhile(Compiler *compiler)
{
    auto substack = compiler->inputBlock(SUBSTACK);
    compiler->addInstruction(vm::OP_UNTIL_LOOP);
    compiler->addInput(CONDITION);
    compiler->addInstruction(vm::OP_NOT);
    compiler->addInstruction(vm::OP_BEGIN_UNTIL_LOOP);
    compiler->moveToSubstack(substack, Compiler::SubstackType::Loop);
}

void ControlBlocks::compileRepeatForEach(Compiler *compiler)
{
    compiler->addInput(VALUE);
    auto substack = compiler->inputBlock(SUBSTACK);
    if (substack) {
        compiler->addInstruction(vm::OP_REPEAT_LOOP);
        compiler->addInstruction(vm::OP_REPEAT_LOOP_INDEX1);
        compiler->addInstruction(vm::OP_SET_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
        compiler->moveToSubstack(substack, Compiler::SubstackType::Loop);
    } else
        compiler->addInstruction(vm::OP_SET_VAR, { compiler->variableIndex(compiler->field(VARIABLE)->valuePtr()) });
}

void ControlBlocks::compileIfStatement(Compiler *compiler)
{
    auto substack = compiler->inputBlock(SUBSTACK);
    if (substack) {
        compiler->addInput(CONDITION);
        compiler->addInstruction(vm::OP_IF);
        compiler->moveToSubstack(substack, Compiler::SubstackType::IfStatement);
    }
}

void ControlBlocks::compileIfElseStatement(Compiler *compiler)
{
    auto substack1 = compiler->inputBlock(SUBSTACK);
    auto substack2 = compiler->inputBlock(SUBSTACK2);
    if (substack1 || substack2)
        compiler->addInput(CONDITION);
    if (substack1 && substack2) {
        compiler->addInstruction(vm::OP_IF);
        compiler->moveToSubstack(substack1, substack2, Compiler::SubstackType::IfStatement);
    } else if (substack1) {
        compiler->addInstruction(vm::OP_IF);
        compiler->moveToSubstack(substack1, Compiler::SubstackType::IfStatement);
    } else if (substack2) {
        compiler->addInstruction(vm::OP_NOT);
        compiler->addInstruction(vm::OP_IF);
        compiler->moveToSubstack(substack2, Compiler::SubstackType::IfStatement);
    }
}

void ControlBlocks::compileStop(Compiler *compiler)
{
    int option = compiler->field(STOP_OPTION)->specialValueId();
    switch (option) {
        case StopAll:
            compiler->addFunctionCall(&stopAll);
            compiler->addInstruction(vm::OP_HALT);
            break;

        case StopThisScript:
            compiler->addInstruction(vm::OP_HALT);
            break;

        case StopOtherScriptsInSprite:
            compiler->addFunctionCall(&stopOtherScriptsInSprite);
            break;
    }
}

void ControlBlocks::compileWait(Compiler *compiler)
{
    compiler->addInput(DURATION);
    compiler->addFunctionCall(&startWait);
    compiler->addFunctionCall(&wait);
}

void ControlBlocks::compileWaitUntil(Compiler *compiler)
{
    compiler->addInstruction(vm::OP_CHECKPOINT);
    compiler->addInput(CONDITION);
    compiler->addFunctionCall(&waitUntil);
}

void ControlBlocks::compileStartAsClone(Compiler *compiler)
{
    compiler->engine()->addCloneInitScript(compiler->block());
}

void ControlBlocks::compileCreateClone(Compiler *compiler)
{
    Input *input = compiler->input(CLONE_OPTION);

    if (input->pointsToDropdownMenu()) {
        std::string spriteName = input->selectedMenuItem();

        if (spriteName == "_myself_")
            compiler->addFunctionCall(&createCloneOfMyself);
        else {
            int index = compiler->engine()->findTarget(spriteName);
            compiler->addConstValue(index);
            compiler->addFunctionCall(&createCloneByIndex);
        }
    } else {
        compiler->addInput(input);
        compiler->addFunctionCall(&createClone);
    }
}

void ControlBlocks::compileDeleteThisClone(Compiler *compiler)
{
    compiler->addFunctionCall(&deleteThisClone);
}

unsigned int ControlBlocks::stopAll(VirtualMachine *vm)
{
    vm->engine()->stop();
    return 0;
}

unsigned int ControlBlocks::stopOtherScriptsInSprite(VirtualMachine *vm)
{
    vm->engine()->stopTarget(vm->target(), vm->thread());
    return 0;
}

unsigned int ControlBlocks::startWait(VirtualMachine *vm)
{
    if (!clock)
        clock = Clock::instance().get();

    auto currentTime = clock->currentSteadyTime();
    m_timeMap[vm] = { currentTime, vm->getInput(0, 1)->toDouble() * 1000 };
    vm->engine()->requestRedraw();

    return 1;
}

unsigned int ControlBlocks::wait(VirtualMachine *vm)
{
    if (!clock)
        clock = Clock::instance().get();

    auto currentTime = clock->currentSteadyTime();
    assert(m_timeMap.count(vm) == 1);
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_timeMap[vm].first).count() >= m_timeMap[vm].second) {
        m_timeMap.erase(vm);
        vm->stop(true, true, false);
    } else
        vm->stop(true, true, true);
    return 0;
}

unsigned int ControlBlocks::waitUntil(VirtualMachine *vm)
{
    if (!vm->getInput(0, 1)->toBool()) {
        vm->moveToLastCheckpoint();
        vm->stop(true, true, false);
    }
    return 1;
}

unsigned int ControlBlocks::createClone(VirtualMachine *vm)
{
    std::string spriteName = vm->getInput(0, 1)->toString();
    Target *target;

    if (spriteName == "_myself_")
        target = vm->target();
    else
        target = vm->engine()->targetAt(vm->engine()->findTarget(spriteName));

    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        sprite->clone();

    return 1;
}

unsigned int ControlBlocks::createCloneByIndex(VirtualMachine *vm)
{
    Target *target = vm->engine()->targetAt(vm->getInput(0, 1)->toInt());
    Sprite *sprite = dynamic_cast<Sprite *>(target);

    if (sprite)
        sprite->clone();

    return 1;
}

unsigned int ControlBlocks::createCloneOfMyself(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite)
        sprite->clone();

    return 0;
}

unsigned int ControlBlocks::deleteThisClone(VirtualMachine *vm)
{
    Sprite *sprite = dynamic_cast<Sprite *>(vm->target());

    if (sprite && sprite->isClone()) {
        vm->engine()->stopTarget(sprite, nullptr);
        sprite->deleteClone();
    }

    return 0;
}
