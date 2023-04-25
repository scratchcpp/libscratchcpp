// SPDX-License-Identifier: Apache-2.0

#include "controlblocks.h"

namespace libscratchcpp
{

std::map<std::pair<RunningScript *, Block *>, std::pair<int, int>> ControlBlocks::repeatLoops;

ControlBlocks::ControlBlocks()
{
    // Blocks
    addBlock("control_forever", &ControlBlocks::repeatForever);
    addBlock("control_repeat", &ControlBlocks::repeat);
    addBlock("control_repeat_until", &ControlBlocks::repeatUntil);
    addBlock("control_if", &ControlBlocks::ifStatement);
    addBlock("control_if_else", &ControlBlocks::ifElseStatement);
    addBlock("control_stop", &ControlBlocks::stop);

    // Inputs
    addInput("SUBSTACK", SUBSTACK);
    addInput("SUBSTACK2", SUBSTACK2);
    addInput("TIMES", TIMES);
    addInput("CONDITION", CONDITION);

    // Fields
    addField("STOP_OPTION", STOP_OPTION);
}

std::string ControlBlocks::name() const
{
    return "Control";
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
        int count = args.input(TIMES)->value().toNumber();
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

Value ControlBlocks::stop(const BlockArgs &args)
{
    std::string option = args.field(STOP_OPTION)->value().toString();
    // TODO: Register special field values and compile them to integers
    if (option == "all")
        args.engine()->stop();
    else if (option == "this script")
        args.engine()->stopScript(args.script());
    else if (option == "other scripts in sprite")
        args.engine()->stopTarget(args.target(), args.script());
    return Value();
}

} // namespace libscratchcpp
