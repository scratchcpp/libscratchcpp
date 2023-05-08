// SPDX-License-Identifier: Apache-2.0

#include "controlblocks.h"
#include "../engine/compiler.h"

namespace libscratchcpp
{

std::map<std::pair<RunningScript *, Block *>, std::pair<int, int>> ControlBlocks::repeatLoops;

ControlBlocks::ControlBlocks()
{
    // Blocks
    addCompileFunction("control_forever", &ControlBlocks::compileRepeatForever);
    addCompileFunction("control_repeat", &ControlBlocks::compileRepeat);
    addCompileFunction("control_repeat_until", &ControlBlocks::compileRepeatUntil);
    addCompileFunction("control_if", &ControlBlocks::compileIfStatement);
    addCompileFunction("control_if_else", &ControlBlocks::compileIfElseStatement);
    addCompileFunction("control_stop", &ControlBlocks::compileStop);
    addBlock("control_forever", &ControlBlocks::repeatForever);
    addBlock("control_repeat", &ControlBlocks::repeat);
    addBlock("control_repeat_until", &ControlBlocks::repeatUntil);
    addBlock("control_if", &ControlBlocks::ifStatement);
    addBlock("control_if_else", &ControlBlocks::ifElseStatement);

    // Inputs
    addInput("SUBSTACK", SUBSTACK);
    addInput("SUBSTACK2", SUBSTACK2);
    addInput("TIMES", TIMES);
    addInput("CONDITION", CONDITION);

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

Value ControlBlocks::repeatForever(const BlockArgs &args)
{
    auto script = args.script();
    if (script->atCMouthEnd()) {
        // Return false to prevent breaking from a forever loop
        return false;
    } else {
        // Move to the C mouth
        script->moveToSubstack(args, SUBSTACK);
        return Value();
    }
}

Value ControlBlocks::repeat(const BlockArgs &args)
{
    auto script = args.script();
    if (script->atCMouthEnd()) {
        Block *cMouth = script->cMouth().get();
        auto loop = repeatLoops[{ script, cMouth }];
        int i = loop.first + 1;
        repeatLoops[{ script, cMouth }] = { i, loop.second };
        if (i >= loop.second) {
            repeatLoops.erase({ script, cMouth });
            return true;
        } else
            return false;
    } else {
        int count = args.input(TIMES)->value().toInt();
        if (count > 0) {
            auto substack = script->getSubstack(args, SUBSTACK);
            if (substack) {
                // Create the loop
                repeatLoops[{ script, script->currentBlock().get() }] = { 0, count };

                script->moveToSubstack(substack);
            }
        }
        return Value();
    }
}

Value ControlBlocks::repeatUntil(const BlockArgs &args)
{
    auto script = args.script();
    bool cond = args.input(CONDITION)->value().toBool();
    if (script->atCMouthEnd())
        return cond;
    else {
        if (cond)
            script->skipSubstack();
        else
            script->moveToSubstack(args, SUBSTACK);
        return Value();
    }
}

Value ControlBlocks::repeatWhile(const BlockArgs &args)
{
    auto script = args.script();
    bool cond = args.input(CONDITION)->value().toBool();
    if (script->atCMouthEnd())
        return !cond;
    else {
        if (cond)
            script->moveToSubstack(args, SUBSTACK);
        else
            script->skipSubstack();
        return Value();
    }
}

Value ControlBlocks::ifStatement(const BlockArgs &args)
{
    auto script = args.script();
    bool cond = args.input(CONDITION)->value().toBool();
    if (script->atCMouthEnd())
        return true;
    else {
        if (cond)
            script->moveToSubstack(args, SUBSTACK);
        else
            script->skipSubstack();
        return Value();
    }
}

Value ControlBlocks::ifElseStatement(const BlockArgs &args)
{
    auto script = args.script();
    bool cond = args.input(CONDITION)->value().toBool();
    if (script->atCMouthEnd())
        return true;
    else {
        if (cond)
            script->moveToSubstack(args, SUBSTACK);
        else
            script->moveToSubstack(args, SUBSTACK2);
        return Value();
    }
}

} // namespace libscratchcpp
