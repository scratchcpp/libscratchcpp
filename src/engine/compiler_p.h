// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

struct CompilerPrivate
{
        CompilerPrivate(IEngine *engine);

        void addInstruction(vm::Opcode opcode, std::initializer_list<unsigned int> args = {});

        void substackEnd();

        IEngine *engine;
        std::shared_ptr<Block> block;
        std::vector<std::pair<std::pair<std::shared_ptr<Block>, std::shared_ptr<Block>>, Compiler::SubstackType>> substackTree;

        std::vector<unsigned int> bytecode;
        std::vector<InputValue *> constValues;
        std::vector<Variable *> variables;
        std::vector<List *> lists;
        std::vector<std::string> procedures;
        std::unordered_map<std::string, std::vector<std::string>> procedureArgs;
        BlockPrototype *procedurePrototype = nullptr;
        bool atomic = true;
        bool warp = false;
};

} // namespace libscratchcpp
