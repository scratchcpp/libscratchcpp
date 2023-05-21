// SPDX-License-Identifier: Apache-2.0

#include "controlblocks.h"
#include "../engine/compiler.h"
#include <cassert>

namespace libscratchcpp
{

ControlBlocks::ControlBlocks()
{
    // Blocks
    addCompileFunction("control_forever", &compileRepeatForever);
    addCompileFunction("control_repeat", &compileRepeat);
    addCompileFunction("control_repeat_until", &compileRepeatUntil);
    addCompileFunction("control_if", &compileIfStatement);
    addCompileFunction("control_if_else", &compileIfElseStatement);
    addCompileFunction("control_stop", &compileStop);
    addCompileFunction("control_wait", &compileWait);
    addCompileFunction("control_wait_until", &compileWaitUntil);

    // Inputs
    addInput("SUBSTACK", SUBSTACK);
    addInput("SUBSTACK2", SUBSTACK2);
    addInput("TIMES", TIMES);
    addInput("CONDITION", CONDITION);
    addInput("DURATION", DURATION);

    // Fields
    addField("STOP_OPTION", STOP_OPTION);

    // Field values
    addFieldValue("all", StopAll);
    addFieldValue("this script", StopThisScript);
    addFieldValue("other scripts in sprite", StopOtherScriptsInSprite);
}

std::string ControlBlocks::name() const
{
    return "Control";
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
    if (substack) {
        compiler->addInstruction(vm::OP_UNTIL_LOOP);
        compiler->addInput(CONDITION);
        compiler->addInstruction(vm::OP_BEGIN_UNTIL_LOOP);
        compiler->moveToSubstack(substack, Compiler::SubstackType::Loop);
    }
}

void ControlBlocks::compileRepeatWhile(Compiler *compiler)
{
    auto substack = compiler->inputBlock(SUBSTACK);
    if (substack) {
        compiler->addInstruction(vm::OP_UNTIL_LOOP);
        compiler->addInput(CONDITION);
        compiler->addInstruction(vm::OP_NOT);
        compiler->addInstruction(vm::OP_BEGIN_UNTIL_LOOP);
        compiler->moveToSubstack(substack, Compiler::SubstackType::Loop);
    }
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

unsigned int ControlBlocks::stopAll(VirtualMachine *vm)
{
    vm->engine()->stop();
    return 0;
}

unsigned int ControlBlocks::stopOtherScriptsInSprite(VirtualMachine *vm)
{
    vm->engine()->stopTarget(vm->target(), vm);
    return 0;
}

unsigned int ControlBlocks::startWait(VirtualMachine *vm)
{
    auto currentTime = std::chrono::steady_clock::now();
    assert(m_timeMap.count(vm) == 0);
    m_timeMap[vm] = { currentTime, vm->getInput(0, 1)->toDouble() * 1000 };
    return 1;
}

unsigned int ControlBlocks::wait(VirtualMachine *vm)
{
    auto currentTime = std::chrono::steady_clock::now();
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

} // namespace libscratchcpp
