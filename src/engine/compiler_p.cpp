// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/block.h>

#include "compiler_p.h"

using namespace libscratchcpp;
using namespace vm;

CompilerPrivate::CompilerPrivate(IEngine *engine, Target *target) :
    engine(engine),
    target(target)
{
    assert(engine);
}

void CompilerPrivate::addInstruction(vm::Opcode opcode, std::initializer_list<unsigned int> args)
{
    bytecode.push_back(opcode);
    for (auto arg : args)
        bytecode.push_back(arg);
}

unsigned int CompilerPrivate::constIndex(InputValue *value, bool pointsToDropdownMenu, const std::string &selectedMenuItem)
{
    auto it = std::find(constValues.begin(), constValues.end(), value);
    if (it != constValues.end())
        return it - constValues.begin();
    constValues.push_back(value);
    constValueMenuInfo[value] = { pointsToDropdownMenu, selectedMenuItem };
    return constValues.size() - 1;
}

void CompilerPrivate::substackEnd()
{
    auto parent = substackTree.back();
    switch (parent.second) {
        case Compiler::SubstackType::Loop:
            // Break the frame at the end of the loop so that other scripts can run within the frame
            // This won't happen in "warp" scripts
            if (!warp)
                addInstruction(OP_BREAK_ATOMIC);
            addInstruction(OP_LOOP_END);
            break;
        case Compiler::SubstackType::IfStatement:
            if (parent.first.second) {
                addInstruction(OP_ELSE);
                block = parent.first.second;
                substackTree[substackTree.size() - 1].first.second = nullptr;
                return;
            } else
                addInstruction(OP_ENDIF);
            break;
    }

    auto parentBlock = parent.first.first;

    if (parentBlock)
        block = parentBlock->next();
    else
        block = nullptr;

    substackTree.pop_back();
    if (!block && !substackTree.empty())
        substackEnd();
}
