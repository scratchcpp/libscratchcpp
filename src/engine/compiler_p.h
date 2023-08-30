// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>

namespace libscratchcpp
{

struct CompilerPrivate
{
        CompilerPrivate(IEngine *engine);

        void addInstruction(vm::Opcode opcode, std::initializer_list<unsigned int> args = {});

        unsigned int constIndex(InputValue *value, bool pointsToDropdownMenu = false, const std::string &selectedMenuItem = "");

        void substackEnd();

        IEngine *engine;
        std::shared_ptr<Block> block;
        std::vector<std::pair<std::pair<std::shared_ptr<Block>, std::shared_ptr<Block>>, Compiler::SubstackType>> substackTree;

        bool initialized = false;

        std::vector<unsigned int> bytecode;
        std::vector<InputValue *> constValues;
        std::unordered_map<InputValue *, std::pair<bool, std::string>> constValueMenuInfo; // input value, <whether the input points to a dropdown menu, selected menu item>
        std::vector<Variable *> variables;
        std::vector<List *> lists;
        std::vector<std::string> procedures;
        std::unordered_map<std::string, std::vector<std::string>> procedureArgs;
        BlockPrototype *procedurePrototype = nullptr;
        bool atomic = true;
        bool warp = false;
};

} // namespace libscratchcpp
