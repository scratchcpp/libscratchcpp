// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/block.h>

#include "compiler_p.h"

using namespace libscratchcpp;
using namespace vm;

CompilerPrivate::CompilerPrivate(IEngine *engine) :
    engine(engine)
{
    assert(engine);
}

void CompilerPrivate::addInstruction(vm::Opcode opcode, std::initializer_list<unsigned int> args)
{
    bytecode.push_back(opcode);
    for (auto arg : args)
        bytecode.push_back(arg);
}

void CompilerPrivate::substackEnd()
{
    auto parent = substackTree.back();
    switch (parent.second) {
        case Compiler::SubstackType::Loop:
            if (!atomic) {
                if (!warp)
                    addInstruction(OP_BREAK_ATOMIC);
                atomic = true;
            }
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
